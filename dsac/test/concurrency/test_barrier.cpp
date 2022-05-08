#include "catch2/catch.hpp"

#include <dsac/concurrency/synchronization/barrier.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <thread>

TEST_CASE("Проверка корректности выполнения Barrier", "[barrier]") {
  using namespace dsac::syncing;

  constexpr std::size_t kNumWorkers = 10u;
  Barrier               barrier{kNumWorkers};

  dsac::dynamic_array<bool>        arrived(kNumWorkers, false);
  dsac::dynamic_array<std::thread> workers;
  for (std::size_t i{}; i < kNumWorkers; ++i) {
    workers.emplace_back(std::thread{[index = i, &arrived, &barrier]() {
      arrived[index] = true;
      barrier.ArriveAndWait();

      const auto is_true = [](bool const& value) { return value; };
      REQUIRE(std::all_of(arrived.begin(), arrived.end(), is_true));
      barrier.ArriveAndWait();

      arrived[index] = false;
      barrier.ArriveAndWait();

      REQUIRE(std::all_of(arrived.begin(), arrived.end(), std::not_fn(is_true)));
      barrier.ArriveAndWait();
    }});
  }

  for (auto& worker : workers) {
    worker.join();
  }
}
