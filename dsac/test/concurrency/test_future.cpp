#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <thread>

TEST_CASE("Execution in various scenarios", "[future][dataflow]") {
  SECTION("Initialization and extraction in one execution thread") {
    dsac::promise<int> promise;
    dsac::future<int>  future = promise.make_future();

    promise.set(10);
    dsac::result<int> value = std::move(future).get();
    REQUIRE(value.has_value());
    REQUIRE(value.value_or_throw() == 10);
  }
  SECTION("Initialization promise object in non-main thread") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

    dsac::promise<int> promise;
    dsac::future<int>  future = promise.make_future();

    executor->submit([promise = std::move(promise)]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      promise.set(10);
    });

    dsac::result<int> value = std::move(future).get();
    REQUIRE(value.has_value());
    REQUIRE(value.value_or_throw() == 10);

    executor->join();
  }
  SECTION("Initialization promise object in main thread") {
    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    dsac::future<bool> future = promise.make_future().then([main_thread_id](const dsac::result<int>& result) {
      REQUIRE(std::this_thread::get_id() == main_thread_id);
      REQUIRE(result.has_value());
      REQUIRE(result.value_or_throw() == 10);
      return true;
    });

    promise.set(10);

    dsac::result<bool> result = std::move(future).get();
    REQUIRE(result.value_or_throw());
  }
  SECTION("Executing a deferred user handler via executor dsac::executor_base_ref") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    dsac::future<bool> future = promise.make_future().via(executor).then([main_thread_id](auto&& result) {
      REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
      REQUIRE(result.has_value());
      REQUIRE(result.value_or_throw() == 10);
      return true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    promise.set(10);

    dsac::result<bool> result = std::move(future).get();
    REQUIRE(result.value_or_throw());

    executor->join();
  }
  SECTION("Execution of fluent interface in a main thread") {
    const auto add_10 = [](const dsac::result<int>& result) {
      REQUIRE(result.has_value());
      return result.value_or_throw() + 10;
    };

    const auto mul_2 = [](const dsac::result<int>& result) {
      REQUIRE(result.has_value());
      return result.value_or_throw() * 2;
    };

    const auto sub_5 = [](const dsac::result<int>& result) {
      REQUIRE(result.has_value());
      return result.value_or_throw() - 5;
    };

    dsac::promise<int> promise;
    dsac::future<int>  future = promise.make_future().then(add_10).then(mul_2).then(sub_5);

    promise.set(10);

    dsac::result<int> result = std::move(future).get();
    REQUIRE(result.has_value());
    REQUIRE(result.value_or_throw() == 35);
  }
  SECTION("Execution of fluent interface in a non-main thread") {
    constexpr std::size_t   kNumberWorkers = 2U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

    std::thread::id const main_thread_id = std::this_thread::get_id();

    dsac::promise<int> promise;
    /* dsac::future<int> future = */ promise.make_future()
        .via(executor)
        .then([main_thread_id](const dsac::result<int>& result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          return result.value_or_throw() + 10;
        })
        .then([main_thread_id](const dsac::result<int>& result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          return result.value_or_throw() * 10;
        })
        .then([main_thread_id](const dsac::result<int>& result) {
          REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);
          REQUIRE(result.has_value());
          REQUIRE(result.value_or_throw() == 200);
          return 0;
        });

    executor->submit([promise = std::move(promise), main_thread_id]() mutable {
      REQUIRE_FALSE(std::this_thread::get_id() == main_thread_id);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      promise.set(10);
    });

    executor->join();
  }
}
