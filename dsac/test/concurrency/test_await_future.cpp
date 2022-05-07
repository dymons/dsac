#include "catch2/catch.hpp"

#include <dsac/concurrency/coroutines/futures/awaiter.hpp>
#include <dsac/concurrency/coroutines/futures/promise.hpp>
#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/futures/async_via.hpp>
#include <dsac/concurrency/futures/future.hpp>

dsac::future<int> coroutine(dsac::base_executor_ptr executor) {
  dsac::future<int> future = dsac::async_via(executor, []() { return 42; });
  co_return co_await std::move(future);
}

TEST_CASE("Stackless coroutine C++", "[coroutine][awaiter]") {
  constexpr const std::size_t kNumberWorkers = 4U;
  dsac::base_executor_ptr     executor       = dsac::make_static_thread_pool(kNumberWorkers);

  auto future = coroutine(executor);
  REQUIRE(std::move(future).Get().ValueOrThrow() == 42);

  executor->join();
}