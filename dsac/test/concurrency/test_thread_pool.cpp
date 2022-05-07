#include "catch2/catch.hpp"

#include <chrono>
#include <dsac/concurrency/executors/static_thread_pool.hpp>

TEST_CASE("Проверка корректности выполнения Thread Pool", "[static_thread_pool]") {
  constexpr std::size_t   kNumberWorkers = 4U;
  dsac::base_executor_ptr executor       = dsac::make_static_thread_pool(kNumberWorkers);

  auto start = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([]() { std::this_thread::sleep_for(std::chrono::milliseconds(750)); });
  }

  executor->join();
  auto end = std::chrono::steady_clock::now();

  REQUIRE((end - start) < std::chrono::seconds(1));
}
