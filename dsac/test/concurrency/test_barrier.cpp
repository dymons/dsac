#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/barrier.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace {

const auto kIsTrue = [](bool const value) { return value; };

}  // namespace

TEST_CASE("Access to a shared resource with a limited number of threads", "[barrier]") {
  constexpr std::size_t kNumberWorkers = 10u;
  dsac::barrier         barrier{kNumberWorkers};
  auto                  executor = dsac::make_static_thread_pool(kNumberWorkers);

  dsac::dynamic_array<bool> arrived(kNumberWorkers, false);
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([index = i, &arrived, &barrier]() {
      arrived[index] = true;
      barrier.arrive_and_wait();

      assert(std::all_of(arrived.begin(), arrived.end(), kIsTrue));
      barrier.arrive_and_wait();

      arrived[index] = false;
      barrier.arrive_and_wait();

      assert(std::all_of(arrived.begin(), arrived.end(), std::not_fn(kIsTrue)));
    });
  }

  executor->join();
}
