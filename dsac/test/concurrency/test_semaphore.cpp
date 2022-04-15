#include "catch2/catch.hpp"

#include <chrono>
#include <dsac/concurrency/synchronization/semaphore.hpp>
#include <thread>

TEST_CASE("Semaphore как Mutex", "[semaphore_like_mutex]") {
  using namespace dsac::syncing;

  constexpr std::size_t kNumberConsumers = 1U;
  Semaphore             sema{kNumberConsumers};

  auto        start   = std::chrono::steady_clock::now();
  std::size_t counter = 0U;
  std::thread consumer1([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++counter;
    sema.Release();
  });

  std::thread consumer2([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++counter;
    sema.Release();
  });

  std::thread consumer3([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++counter;
    sema.Release();
  });

  std::thread consumer4([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++counter;
    sema.Release();
  });

  consumer1.join();
  consumer2.join();
  consumer3.join();
  consumer4.join();
  auto end = std::chrono::steady_clock::now();

  REQUIRE(counter == 4U);
  REQUIRE((end - start) >= std::chrono::milliseconds(400));
}

TEST_CASE("Ограниченное количество Consumers", "[semaphore_limited]") {
  using namespace dsac::syncing;

  constexpr std::size_t kNumberConsumers = 4U;
  Semaphore             sema{kNumberConsumers};

  auto        start = std::chrono::steady_clock::now();
  std::thread consumer1([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sema.Release();
  });

  std::thread consumer2([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sema.Release();
  });

  std::thread consumer3([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sema.Release();
  });

  std::thread consumer4([&]() {
    sema.Acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sema.Release();
  });

  consumer1.join();
  consumer2.join();
  consumer3.join();
  consumer4.join();
  auto end = std::chrono::steady_clock::now();

  REQUIRE((end - start) <= std::chrono::milliseconds(150));
}