#include <dsac/concurrency/fibers/details/scheduler.hpp>
#include <dsac/concurrency/fibers/go.hpp>

namespace dsac {

auto go(fiber_routine routine) -> void {
  if (auto* scheduler = fiber_scheduler::current(); nullptr != scheduler) {
    scheduler->submit(fiber_scheduler::child_routine{std::move(routine)});
    return;
  }

  auto scheduler = fiber_scheduler::make();
  scheduler.submit(fiber_scheduler::entry_routine{std::move(routine)});
}

}  // namespace dsac
