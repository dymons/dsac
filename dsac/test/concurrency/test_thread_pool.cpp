#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <chrono>
#include <thread>

TEST_CASE("Execution multiple user tasks in parallel", "[static_thread_pool]") {
  constexpr std::size_t   kNumberWorkers = 4U;
  dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

  auto start = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([]() { std::this_thread::sleep_for(std::chrono::milliseconds(750)); });
  }

  executor->join();
  REQUIRE((std::chrono::steady_clock::now() - start) < std::chrono::seconds(1));
}
