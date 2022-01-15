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
    REQUIRE(value.HasValue());
    REQUIRE(value.Get() == 10);

    executor->Join();
  }
  SECTION("Подписка на результат Future и выполнение Callback в потоке Main") {
    std::thread::id const main_thread_id = std::this_thread::get_id();

    Promise<int> promise;
    Future<int> future =
        promise.MakeFuture().Subscribe([main_thread_id](Try<int> result) {
          REQUIRE(std::this_thread::get_id() == main_thread_id);
          REQUIRE(result.HasValue());
          REQUIRE(result.Get() == 10);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    promise.Set(10);

    Try<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.Get() == 10);
  }
  SECTION("Подписка на результат Future и выполнение Callback в потоке Worker") {
    constexpr std::size_t kNumberWorkers = 2U;
    IExecutorPtr executor = StaticThreadPool::Make(kNumberWorkers);

    std::thread::id const main_thread_id = std::this_thread::get_id();

    Promise<int> promise;
    Future<int> future =
        promise.MakeFuture().Via(executor).Subscribe([main_thread_id](Try<int> result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          REQUIRE(result.HasValue());
          REQUIRE(result.Get() == 10);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    promise.Set(10);

    Try<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.Get() == 10);

    executor->Join();
  }
  SECTION("Выполнение последовательности цепочек через функцию Then") {
    const auto add_10 = [](Try<int> result) {
      REQUIRE(result.HasValue());
      return result.Get() + 10;
    };

    const auto mul_2 = [](Try<int> result) {
      REQUIRE(result.HasValue());
      return result.Get() * 2;
    };

    const auto sub_5 = [](Try<int> result) {
      REQUIRE(result.HasValue());
      return result.Get() - 5;
    };

    Promise<int> promise;
    Future<int> future = promise.MakeFuture().Then(add_10).Then(mul_2).Then(sub_5);

    promise.Set(10);

    Try<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.Get() == 35);
  }
}
