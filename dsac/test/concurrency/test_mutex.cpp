#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/mutex.hpp>
#include <dsac/concurrency/synchronization/unique_lock.hpp>

TEST_CASE("Verification of guarantees safety and liveness for mutex", "[mutex]") {
  constexpr std::size_t   kNumberWorkers = 2U;
  constexpr std::size_t   kIterations    = 1000U;
  dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

  dsac::mutex mutex;
  std::size_t counter{};
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([&mutex, &counter]() {
      for (std::size_t i{}; i < kIterations; ++i) {
        dsac::unique_lock guard(mutex);
        counter += 1;
      }
    });
  }

  executor->join();
  REQUIRE(counter == (kNumberWorkers * kIterations));
}
