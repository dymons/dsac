#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/fiber.hpp>
#include <dsac/concurrency/fibers/details/scheduler.hpp>
#include <dsac/functional/defer.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler{};

}  // namespace

namespace dsac {

auto fiber_scheduler::make() -> fiber_scheduler {
  return {};
}

auto fiber_scheduler::submit(entry_routine routine) -> void {
  auto const scheduler_defer = defer([this_ = this]() { kScheduler = this_; }, []() { kScheduler = nullptr; });

  submit(child_routine{std::move(routine).get()});
  while (not fibers_.empty()) {
    fiber* fiber = fibers_.pop_front();
    switch_to(fiber);
    dispatch_of(fiber);
  }
}

auto fiber_scheduler::submit(child_routine routine) -> void {
  fibers_.push_back(fiber::make(kScheduler, stacks_.allocate(), std::move(routine).get()));
}

auto fiber_scheduler::switch_to(fiber* fiber) -> void {
  auto const fiber_defer = defer([&, this]() { current_fiber_ = fiber; }, [this]() { current_fiber_ = nullptr; });

  execution_context_.switch_to(current_fiber_->get_execution_context());
}

auto fiber_scheduler::dispatch_of(fiber* fiber) -> void {
  delete fiber;
}

auto fiber_scheduler::terminate() -> void {
  stacks_.release(std::move(current_fiber_->get_stack()));
  current_fiber_->get_execution_context().switch_to(execution_context_);
}

fiber_scheduler* fiber_scheduler::current() {
  return kScheduler;
}

}  // namespace dsac