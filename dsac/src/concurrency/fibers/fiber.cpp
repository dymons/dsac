#include <dsac/concurrency/fibers/fiber.hpp>

namespace dsac {

fiber::fiber(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine)
  : fiber_scheduler_(scheduler)
  , fiber_routine_(std::move(routine))
  , fiber_state_(fiber_state::starting)
  , fiber_stack_(stack)
  , fiber_execution_context_() {
  fiber_execution_context_.setup(fiber_stack_.view(), this);
}

fiber* fiber::make(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine) {
  return new fiber{scheduler, stack, std::move(routine)};
}

auto fiber::set_state(fiber_state state) noexcept -> void {
  fiber_state_ = state;
}

auto fiber::get_execution_context() & noexcept -> execution_context& {
  return fiber_execution_context_;
}

void fiber::run() noexcept {
  try {
    fiber_routine_();
  } catch (...) {
    // ¯\_(ツ)_/¯
  }

  fiber_scheduler_->terminate();

  std::abort();
}

}  // namespace dsac