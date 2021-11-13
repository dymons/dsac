#include <catch2/catch.hpp>

#include <concurrency/syncing/barrier.hpp>
#include <thread>
#include <memory>
#include <vector>

TEST_CASE("Проверка корректности выполнения Barrier", "[barrier]") {
  using namespace algo::syncing;

  constexpr std::size_t kNumWorkers = 10u;
  Barrier barrier{kNumWorkers};

  std::vector<bool> arrived(kNumWorkers, false);
  std::vector<std::thread> workers;
  for (std::size_t i{}; i < kNumWorkers; ++i) {
    workers.push_back(std::thread{[index = i, &arrived, &barrier]() {
      arrived[index] = true;
      barrier.ArriveAndWait();
      const auto is_true = [](bool&& v) {
        return v;
      };
      REQUIRE(std::all_of(arrived.begin(), arrived.end(), is_true));
    }});
  }

  for (auto& worker : workers) {
    worker.join();
  }
}
