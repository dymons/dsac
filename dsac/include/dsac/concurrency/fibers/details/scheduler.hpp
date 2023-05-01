#pragma once

#include <dsac/concurrency/fibers/details/routine/routine.hpp>
#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/stack/stack.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

class fiber;

class fiber_scheduler final {
public:
  // Constructors

  [[nodiscard]] static auto make() -> fiber_scheduler;

  // Modifiers

  auto running_entry_routing(fiber_routine entry_routine) -> void;

  auto terminate() -> void;

  auto submit_routing(fiber_routine routine) -> void;

  // Observers

  static fiber_scheduler* get_current_scheduler();

private:

  auto switch_to(fiber* fiber) -> void;
  auto dispatch_of(fiber* fiber) -> void;

  fiber*                 current_fiber_{};
  intrusive::list<fiber> fibers_{};
  execution_context      execution_context_{};
  fiber_stack_allocator  stacks_{};
};

}  // namespace dsac
