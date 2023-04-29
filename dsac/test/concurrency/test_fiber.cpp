#include <catch2/catch.hpp>

#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/container/intrusive/list.hpp>

TEST_CASE("Fiber scheduler", "[fibers]") {
  auto scheduler = dsac::fiber_scheduler::make();
  scheduler.running_entry_routing([]() -> void {});
}
