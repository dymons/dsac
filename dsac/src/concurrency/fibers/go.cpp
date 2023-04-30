#include <dsac/concurrency/fibers/details/scheduler.hpp>
#include <dsac/concurrency/fibers/go.hpp>

namespace dsac {

auto go(fiber_routine entry_routine) -> void {
  auto scheduler = fiber_scheduler::make();
  scheduler.running_entry_routing(std::move(entry_routine));
}

}  // namespace dsac
