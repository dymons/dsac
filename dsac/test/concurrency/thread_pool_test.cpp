#include "catch2/catch.hpp"

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <chrono>

TEST_CASE("Проверка корректности выполнения Thread Pool", "[static_thread_pool]") {
  using namespace dsac::concurrency;

  constexpr std::size_t kNumberWorkers = 4U;
  IExecutorPtr executor = StaticThreadPool::Make(kNumberWorkers);

  auto start = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->Submit([]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(750));
    });
  }

  executor->Join();
  auto end = std::chrono::steady_clock::now();

  REQUIRE((end - start) < std::chrono::seconds(1));
}
