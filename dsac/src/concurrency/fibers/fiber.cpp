#include <dsac/concurrency/fibers/fiber.hpp>

namespace dsac {

fiber::fiber(fiber_routine routine)
  : fiber_routine_(std::move(routine))
  , fiber_state_(fiber_state::starting)
  , fiber_execution_context_() {
}

fiber* fiber::make(fiber_routine routine) {
  return new fiber{std::move(routine)};
}

auto fiber::set_state(fiber_state state) noexcept -> void {
  fiber_state_ = state;
}

auto fiber::get_execution_context() & noexcept -> execution_context& {
  return fiber_execution_context_;
}

}  // namespace dsac