#include <dsac/concurrency/fibers/scheduler.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler;

}  // namespace

namespace dsac {

auto fiber_scheduler::main(fiber_routine entry_routine) & -> void {
  struct fiber_scheduler_guard final {
    explicit fiber_scheduler_guard(fiber_scheduler* scheduler) noexcept {
      kScheduler = scheduler;
    }
    ~fiber_scheduler_guard() noexcept {
      kScheduler = nullptr;
    }

    auto operator->() noexcept -> fiber_scheduler* {
      return kScheduler;
    }
  };

  fiber_scheduler_guard(this)->submit(std::move(entry_routine))->execute();
}

auto fiber_scheduler::submit(fiber_routine routine) -> fiber_scheduler* {
  auto* fiber_ = new fiber{std::move(routine)};
  fibers_.push_back(fiber_);
  return this;
}

auto fiber_scheduler::execute() -> void {
  while (not fibers_.empty()) {
    delete fibers_.pop_front();
  }
}

}  // namespace dsac