#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/critical_section.hpp>
#include <dsac/concurrency/synchronization/unique_lock.hpp>

TEST_CASE("Verification of guarantees safety and liveness for spin lock", "[critical_section]") {
  constexpr std::size_t   kNumberWorkers = 2U;
  constexpr std::size_t   kIterations    = 1000U;
  dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

  dsac::critical_section spin_lock;
  std::size_t            counter{};
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([&spin_lock, &counter]() {
      for (int i{}; i < kIterations; ++i) {
        dsac::unique_lock guard(spin_lock);
        counter += 1;
      }
    });
  }

  executor->join();
  REQUIRE(counter == (kNumberWorkers * kIterations));
}
