#include "catch2/catch.hpp"

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/async_via.hpp>
#include <concurrency/executors/static_thread_pool.hpp>
#include <concurrency/coroutines/futures/promise.hpp>
#include <concurrency/coroutines/futures/awaiter.hpp>

using namespace algo::futures;
using namespace algo::concurrency;

Future<int> Coroutine(IExecutorPtr executor) {
  Future<int> future = AsyncVia(executor, []() {
    return 42;
  });
  co_return co_await std::move(future);
}

TEST_CASE("Stackless coroutine C++", "[awaiter]") {
  constexpr const std::size_t kNumberWorkers = 4U;
  auto executor(StaticThreadPool::Make(kNumberWorkers));

  auto future = Coroutine(executor);
  REQUIRE(std::move(future).Get().ValueOrThrow() == 42);

  executor->Join();
}