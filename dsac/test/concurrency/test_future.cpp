#include "catch2/catch.hpp"

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

TEST_CASE("Проверка корректности Future&Promise", "[future_and_promise]") {
  SECTION("Выполнение Future&Promise в однопоточном исполнении") {
    dsac::promise<int> promise;
    dsac::future<int>  future = promise.MakeFuture();

    promise.Set(10);
    dsac::result<int> value = std::move(future).Get();
    REQUIRE(value.HasValue());
    REQUIRE(value.ValueOrThrow() == 10);
  }
  SECTION("Выполнение Future&Promise в разных потоках исполнения") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

    dsac::promise<int> promise;
    dsac::future<int>  future = promise.MakeFuture();

    executor->submit([promise = std::move(promise)]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      promise.Set(10);
    });

    dsac::result<int> value = std::move(future).Get();
    REQUIRE(value.HasValue());
    REQUIRE(value.ValueOrThrow() == 10);

    executor->join();
  }
  SECTION("Подписка на результат Future и выполнение Callback в потоке Main") {
    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    dsac::future<int>  future = promise.MakeFuture().Subscribe([main_thread_id](const dsac::result<int>& result) {
      REQUIRE(std::this_thread::get_id() == main_thread_id);
      REQUIRE(result.HasValue());
      REQUIRE(result.ValueOrThrow() == 10);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    promise.Set(10);

    dsac::result<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.ValueOrThrow() == 10);
  }
  SECTION("Подписка на результат Future и выполнение Callback в потоке Worker") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    dsac::future<int>  future =
        promise.MakeFuture().Via(executor).Subscribe([main_thread_id](const dsac::result<int>& result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          REQUIRE(result.HasValue());
          REQUIRE(result.ValueOrThrow() == 10);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    promise.Set(10);

    dsac::result<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.ValueOrThrow() == 10);

    executor->join();
  }
  SECTION("Выполнение последовательности цепочек через функцию Then") {
    const auto add_10 = [](const dsac::result<int>& result) {
      REQUIRE(result.HasValue());
      return result.ValueOrThrow() + 10;
    };

    const auto mul_2 = [](const dsac::result<int>& result) {
      REQUIRE(result.HasValue());
      return result.ValueOrThrow() * 2;
    };

    const auto sub_5 = [](const dsac::result<int>& result) {
      REQUIRE(result.HasValue());
      return result.ValueOrThrow() - 5;
    };

    dsac::promise<int> promise;
    dsac::future<int>  future = promise.MakeFuture().Then(add_10).Then(mul_2).Then(sub_5);

    promise.Set(10);

    dsac::result<int> result = std::move(future).Get();
    REQUIRE(result.HasValue());
    REQUIRE(result.ValueOrThrow() == 35);
  }
  SECTION("Выполнение последовательности цепочек в потоке Worker") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    /* dsac::future<int> future = */ promise.MakeFuture()
        .Via(executor)
        .Then([main_thread_id](dsac::result<int> result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          return result.ValueOrThrow() + 10;
        })
        .Then([main_thread_id](dsac::result<int> result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          return result.ValueOrThrow() * 10;
        })
        .Then([main_thread_id](dsac::result<int> result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          REQUIRE(result.HasValue());
          REQUIRE(result.ValueOrThrow() == 200);
          return 0;
        });

    executor->submit([promise = std::move(promise), main_thread_id]() mutable {
      REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      promise.Set(10);
    });

    executor->join();
  }
}
