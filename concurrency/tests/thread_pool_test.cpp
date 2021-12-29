#include <catch2/catch.hpp>

#include <concurrency/thread_pool/static_thread_pool.hpp>
#include <chrono>

TEST_CASE("Проверка корректности выполнения Thread Pool", "[static_thread_pool]") {
  using namespace algo::concurrency;

  constexpr std::size_t kNumberWorkers = 4U;
  StaticThreadPool workers{kNumberWorkers};

  auto start = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    workers.Submit([]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(750));
    });
  }

  workers.Join();
  auto end = std::chrono::steady_clock::now();

  REQUIRE((end - start) < std::chrono::seconds(1));
}
