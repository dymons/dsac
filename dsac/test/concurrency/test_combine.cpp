#include "catch2/catch.hpp"

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/concurrency/futures/combine/first_of.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <chrono>

TEST_CASE("Проверка корректности комбинатора на Future", "[combine]") {
  SECTION("Проверка исполнения комбинатора FirstOf") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

    std::vector<dsac::future<int>> futures;
    {
      dsac::promise<int> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        promise.Set(1);
      });
    }

    {
      dsac::promise<int> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        promise.Set(2);
      });
    }

    dsac::future<int> future = dsac::first_of(std::move(futures));
    dsac::result<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.ValueOrThrow() == 2);

    executor->join();
  }
  SECTION("Проверка исполнения комбинатора FirstN") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

    constexpr std::size_t                  kNumberOfTasks = 10U;
    std::vector<dsac::future<std::size_t>> futures;
    futures.reserve(kNumberOfTasks);
    for (std::size_t i{}; i < kNumberOfTasks; ++i) {
      dsac::promise<std::size_t> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->submit([promise = std::move(promise), i]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(i * 10U));
        promise.Set(i);
      });
    }

    constexpr std::size_t                                kQuorum = 4U;
    dsac::future<std::vector<dsac::result<std::size_t>>> future  = dsac::first_n(std::move(futures), kQuorum);
    dsac::result<std::vector<dsac::result<std::size_t>>> result  = std::move(future).Get();
    REQUIRE(result.HasValue());

    std::vector<dsac::result<std::size_t>> sequence = result.ValueOrThrow();
    REQUIRE(sequence.size() == kQuorum);

    std::vector<dsac::result<std::size_t>> expected(kQuorum, dsac::result<std::size_t>(0U));
    std::generate(expected.begin(), expected.end(), [n = 0U]() mutable { return dsac::result<std::size_t>{n++}; });
    REQUIRE(sequence == expected);

    executor->join();
  }
}
