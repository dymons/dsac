#include <dsac/concurrency/fibers/details/fiber.hpp>
#include <dsac/concurrency/fibers/details/scheduler.hpp>

namespace dsac {

fiber::fiber(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine)
  : fiber_scheduler_(scheduler)
  , fiber_routine_(std::move(routine))
  , fiber_stack_(std::move(stack))
  , fiber_execution_context_() {
  fiber_execution_context_.setup(fiber_stack_.view(), this);
}

fiber* fiber::make(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine) {
  return new fiber{scheduler, std::move(stack), std::move(routine)};
}

auto fiber::get_execution_context() & noexcept -> execution_context& {
  return fiber_execution_context_;
}

auto fiber::get_stack() & noexcept -> fiber_stack& {
  return fiber_stack_;
}

void fiber::run() noexcept {
  try {
    fiber_routine_();
  } catch (...) {
    // ¯\_(ツ)_/¯
  }

  fiber_execution_context_.switch_to(fiber_scheduler_->execution_context_);

  std::abort();
}

}  // namespace dsac