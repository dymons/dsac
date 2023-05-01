#include <dsac/concurrency/fibers/details/scheduler.hpp>
#include <dsac/concurrency/fibers/go.hpp>

namespace dsac {

auto go(fiber_routine routine) -> void {
  if (auto* scheduler = fiber_scheduler::current(); nullptr != scheduler) {
    scheduler->submit_routing(std::move(routine));
    return;
  }

  auto scheduler = fiber_scheduler::make();
  scheduler.running_entry_routing(std::move(routine));
}

}  // namespace dsac
