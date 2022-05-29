#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/concurrency/futures/combine/first_of.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <chrono>

TEST_CASE("Checking the execution of combinators over dsac::future", "[future][combine]") {
  SECTION("Use a combinator dsac::first_of to get the first value of calculation") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

    dsac::dynamic_array<dsac::future<int>> futures;
    {
      dsac::promise<int> promise;
      futures.emplace_back(promise.make_future().via(executor));
      executor->submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        promise.set(1);
      });
    }

    {
      dsac::promise<int> promise;
      futures.emplace_back(promise.make_future().via(executor));
      executor->submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        promise.set(2);
      });
    }

    dsac::future<int> future = dsac::first_of(std::move(futures));
    dsac::result<int> result = std::move(future).get();
    REQUIRE(result.has_value());
    REQUIRE(result.value_or_throw() == 2);

    executor->join();
  }
  SECTION("Use a combinator dsac::first_n to get the first N value of calculation") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

    constexpr std::size_t                          kNumberOfTasks = 10U;
    dsac::dynamic_array<dsac::future<std::size_t>> futures;
    futures.reserve(kNumberOfTasks);
    for (std::size_t i{}; i < kNumberOfTasks; ++i) {
      dsac::promise<std::size_t> promise;
      futures.emplace_back(promise.make_future().via(executor));
      executor->submit([promise = std::move(promise), i]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(i * 10U));
        promise.set(i);
      });
    }

    constexpr std::size_t                                        kQuorum = 4U;
    dsac::future<dsac::dynamic_array<dsac::result<std::size_t>>> future  = dsac::first_n(std::move(futures), kQuorum);
    dsac::result<dsac::dynamic_array<dsac::result<std::size_t>>> result  = std::move(future).get();
    REQUIRE(result.has_value());

    dsac::dynamic_array<dsac::result<std::size_t>> sequence = result.value_or_throw();
    REQUIRE(sequence.size() == kQuorum);

    dsac::dynamic_array<dsac::result<std::size_t>> expected(kQuorum, dsac::result<std::size_t>(0U));
    std::generate(expected.begin(), expected.end(), [n = 0U]() mutable { return dsac::result<std::size_t>{n++}; });
    REQUIRE(sequence == expected);

    executor->join();
  }
}
