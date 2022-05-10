#include <catch2/catch.hpp>

#include <dsac/concurrency/synchronization/atomic.hpp>

TEST_CASE("Create SpinLock on dsac::atomic", "[atomic]") {
  dsac::atomic<std::int64_t> atomic;
  atomic.store(1);
  REQUIRE(atomic.load() == 1);
}
