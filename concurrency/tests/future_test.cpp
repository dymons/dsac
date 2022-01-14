#include <catch2/catch.hpp>

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

#include <concurrency/executors/static_thread_pool.hpp>

TEST_CASE("Проверка корректности Future&Promise", "[future_and_promise]") {
  using namespace algo::futures;
  using namespace algo::concurrency;

  SECTION("Выполнение Future&Promise в однопоточном исполнении") {
    Promise<int> promise;
    Future<int> future = promise.MakeFuture();

    promise.Set(10);
    Try<int> value = std::move(future).Get();
    REQUIRE_FALSE(value.HasException());
    REQUIRE(value.HasValue());
    REQUIRE(value.Get() == 10);
  }
  SECTION("Выполнение Future&Promise в разных потоках исполнения") {
    constexpr std::size_t kNumberWorkers = 2U;
    IExecutorPtr executor = StaticThreadPool::Make(kNumberWorkers);

    Promise<int> promise;
    Future<int> future = promise.MakeFuture();

    executor->Submit([promise = std::move(promise)]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      promise.Set(10);
    });

    Try<int> value = std::move(future).Get();
    REQUIRE_FALSE(value.HasException());
    REQUIRE(value.HasValue());
    REQUIRE(value.Get() == 10);

    executor->Join();
  }
}
