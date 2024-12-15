#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/semaphore.hpp>
#include <dsac/concurrency/synchronization/unique_lock.hpp>

#include <chrono>
#include <thread>

TEST_CASE("Implement Mutex primitive based on semaphore", "[semaphore][mutex]") {
  class smutex final {
    dsac::semaphore sema_{1U};

  public:
    void lock() {
      sema_.acquire();
    }

    void unlock() {
      sema_.release();
    }
  };

  constexpr std::size_t kNumberWorkers = 4U;
  auto                  executor       = dsac::make_static_thread_pool(kNumberWorkers);

  smutex                                mutex;
  std::size_t                           counter = 0;
  std::chrono::steady_clock::time_point start   = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberWorkers; ++i) {
    executor->submit([&mutex, &counter]() {
      dsac::unique_lock guard(mutex);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      ++counter;
    });
  }

  executor->join();

  REQUIRE(counter == kNumberWorkers);
  REQUIRE((std::chrono::steady_clock::now() - start) >= std::chrono::milliseconds(400));
}

TEST_CASE("Access to a shared resource of a fixed number of threads", "[semaphore][limited]") {
  class mmutex final {
    dsac::semaphore sema_;

  public:
    explicit mmutex(std::size_t consumers)
      : sema_{consumers} {
    }

    void lock() {
      sema_.acquire();
    }

    void unlock() {
      sema_.release();
    }
  };

  constexpr std::size_t kNumberConsumers = 4U;
  mmutex                mutex{kNumberConsumers};
  auto                  executor = dsac::make_static_thread_pool(kNumberConsumers);

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  for (std::size_t i{}; i < kNumberConsumers; ++i) {
    executor->submit([&mutex]() {
      dsac::unique_lock guard(mutex);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
  }

  executor->join();
  REQUIRE((std::chrono::steady_clock::now() - start) <= std::chrono::milliseconds(150));
}
