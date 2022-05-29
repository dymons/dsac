#include <catch2/catch.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/synchronization/mvar.hpp>

#include <chrono>
#include <thread>

TEST_CASE("Testcases for checking safety and liveness guarantees", "[mvar]") {
  static_assert(!std::is_copy_constructible<dsac::mvar<int>>::value);
  static_assert(!std::is_move_constructible<dsac::mvar<int>>::value);

  SECTION("Default construction is empty") {
    dsac::mvar<int> mvar;
    REQUIRE(mvar.is_empty());
  }
  SECTION("Construction with predefined data") {
    dsac::mvar<int> mvar{10};
    REQUIRE_FALSE(mvar.is_empty());
    REQUIRE(mvar.read_only() == 10);
  }
  SECTION("Initializing dsac::mvar in non-main thread") {
    dsac::mvar<int> mvar;

    constexpr std::size_t   kNumberWorkers = 1U;
    dsac::executor_base_ref executor       = dsac::make_static_thread_pool(kNumberWorkers);
    executor->submit([&mvar] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mvar.put(10);
    });

    REQUIRE(mvar.is_empty());
    REQUIRE(mvar.take() == 10);
    REQUIRE(mvar.is_empty());

    executor->join();
  }
}
