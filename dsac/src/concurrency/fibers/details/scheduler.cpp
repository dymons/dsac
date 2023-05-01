#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/fiber.hpp>
#include <dsac/concurrency/fibers/details/scheduler.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler{};

}  // namespace

namespace dsac {

auto fiber_scheduler::make() -> fiber_scheduler {
  return {};
}

auto fiber_scheduler::running_entry_routing(fiber_routine entry_routine) -> void {
  submit_routing(std::move(entry_routine));
  while (not fibers_.empty()) {
    fiber* fiber = fibers_.pop_front();
    switch_to(fiber);
    dispatch_of(fiber);
  }
}

auto fiber_scheduler::switch_to(fiber* fiber) -> void {
  current_fiber_ = fiber;
  execution_context_.switch_to(current_fiber_->get_execution_context());
  current_fiber_ = nullptr;
}

auto fiber_scheduler::dispatch_of(fiber* fiber) -> void {
  delete fiber;
}

auto fiber_scheduler::terminate() -> void {
  stacks_.release(std::move(current_fiber_->get_stack()));
  current_fiber_->get_execution_context().switch_to(execution_context_);
}

auto fiber_scheduler::submit_routing(fiber_routine routine) -> void {
  fibers_.push_back(fiber::make(kScheduler, stacks_.allocate(), std::move(routine)));
}

fiber_scheduler* fiber_scheduler::get_current_scheduler() {
  return kScheduler;
}

}  // namespace dsac