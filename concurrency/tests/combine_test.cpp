#include <catch2/catch.hpp>

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>
#include <concurrency/executors/static_thread_pool.hpp>
#include <concurrency/futures/combine/first_of.hpp>

#include <chrono>

TEST_CASE("Проверка корректности комбинатора на Future", "[combine]") {
  using namespace algo::futures;
  using namespace algo::concurrency;

  SECTION("Проверка исполнения комбинатора FirstOf") {
    constexpr std::size_t kNumberWorkers = 2U;
    IExecutorPtr executor = StaticThreadPool::Make(kNumberWorkers);

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
    Try<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.Get() == 2);

    executor->Join();
  }
}
