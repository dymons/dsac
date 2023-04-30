#include <catch2/catch.hpp>

#include <dsac/concurrency/fibers/scheduler.hpp>

TEST_CASE("Fiber scheduler", "[fibers]") {
  auto executed = bool{};

  auto scheduler = dsac::fiber_scheduler::make();
  scheduler.running_entry_routing([&executed] -> void { executed = true; });

  REQUIRE(executed);
}
