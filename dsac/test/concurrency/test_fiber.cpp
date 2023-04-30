#include <catch2/catch.hpp>

#include <dsac/concurrency/fibers/scheduler.hpp>

TEST_CASE("Fiber scheduler", "[fibers]") {
  bool executed = false;

  auto scheduler = dsac::fiber_scheduler::make();
  scheduler.running_entry_routing([&executed] -> void { executed = true; });

  REQUIRE(executed);
}
