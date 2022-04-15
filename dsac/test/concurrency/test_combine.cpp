#include "catch2/catch.hpp"

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/concurrency/futures/combine/first_of.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <chrono>

TEST_CASE("Проверка корректности комбинатора на Future", "[combine]") {
  using namespace dsac::futures;
  using namespace dsac::concurrency;

  SECTION("Проверка исполнения комбинатора FirstOf") {
    constexpr std::size_t kNumberWorkers = 2U;
    IExecutorPtr          executor = StaticThreadPool::Make(kNumberWorkers);

    std::vector<Future<int>> futures;
    {
      Promise<int> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->Submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        promise.Set(1);
      });
    }

    {
      Promise<int> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->Submit([promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        promise.Set(2);
      });
    }

    Future<int> future = FirstOf(std::move(futures));
    Try<int>    result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.ValueOrThrow() == 2);

    executor->Join();
  }
  SECTION("Проверка исполнения комбинатора FirstN") {
    constexpr std::size_t kNumberWorkers = 2U;
    IExecutorPtr          executor = StaticThreadPool::Make(kNumberWorkers);

    constexpr std::size_t            kNumberOfTasks = 10U;
    std::vector<Future<std::size_t>> futures;
    futures.reserve(kNumberOfTasks);
    for (std::size_t i{}; i < kNumberOfTasks; ++i) {
      Promise<std::size_t> promise;
      futures.push_back(promise.MakeFuture().Via(executor));
      executor->Submit([promise = std::move(promise), i]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(i * 10U));
        promise.Set(i);
      });
    }

    constexpr std::size_t                 kQuorum = 4U;
    Future<std::vector<Try<std::size_t>>> future =
        FirstN(std::move(futures), kQuorum);
    Try<std::vector<Try<std::size_t>>> result = std::move(future).Get();
    REQUIRE(result.HasValue());

    std::vector<Try<std::size_t>> sequence = result.ValueOrThrow();
    REQUIRE(sequence.size() == kQuorum);

    std::vector<Try<std::size_t>> expected(kQuorum, Try<std::size_t>(0U));
    std::generate(expected.begin(), expected.end(), [n = 0U]() mutable {
      return Try<std::size_t>{n++};
    });
    REQUIRE(sequence == expected);

    executor->Join();
  }
}
