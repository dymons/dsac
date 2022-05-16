#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/atomic.hpp>
#include <dsac/container/dynamic_array.hpp>
#include <thread>
TEST_CASE("Store and load on atomic data", "[atomic][store]") {
  dsac::atomic<std::int64_t> atomic;
  REQUIRE(atomic.load() == 0);
  atomic.store(1);
  REQUIRE(atomic.load() == 1);
}

TEST_CASE("Exchange data over atomic storage", "[atomic][store]") {
  SECTION("Exchange on a default atomic storage") {
    dsac::atomic<std::int64_t> atomic;
    REQUIRE(atomic.exchange(1) == 0);
  }
  SECTION("Exchange a sequence of data on atomic storage") {
    dsac::atomic<std::int64_t> atomic;
    constexpr std::int64_t     kIterations = 10;
    for (std::int64_t expected = atomic.load(), next = expected + 1; expected < kIterations; ++expected, ++next) {
      REQUIRE(atomic.exchange(next) == expected);
    }
  }
}

TEST_CASE("Create a simple Spin Lock", "[atomic][spin_lock]") {
  class spin_lock final {
    dsac::atomic<std::int64_t> locked_;
    dsac::atomic<std::int64_t> subscribers_;

  public:
    void lock() noexcept {
      while (locked_.exchange(1) != 0) {
        while (locked_.load() != 0) {
        }
      }
      REQUIRE(get_subscribers() == 0);
      subscribers_.store(subscribers_.load() + 1);
    }

    void unlock() noexcept {
      REQUIRE(get_subscribers() == 1);
      subscribers_.store(subscribers_.load() - 1);
      locked_.store(0);
    }

    [[nodiscard]] bool is_acquired() const noexcept {
      return locked_.load() == 1U;
    }

    [[nodiscard, gnu::always_inline]] std::int64_t get_subscribers() const noexcept {
      return subscribers_.load();
    }
  };
  
  constexpr std::size_t   kNumberWorkers = 4U;
  constexpr std::int64_t  kIterations    = 2000;
  dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);

  spin_lock                 mutex;
  std::vector<std::int64_t> shared_data;
  for (std::int64_t i{}; i < kIterations; ++i) {
    executor->submit([&, data = i]() {
      std::lock_guard guard{mutex};
      REQUIRE(mutex.is_acquired());
      shared_data.push_back(data);
    });
  }
  executor->join();
  REQUIRE(shared_data.size() == kIterations);
}
