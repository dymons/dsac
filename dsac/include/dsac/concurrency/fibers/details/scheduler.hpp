#pragma once

#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/routine/routine.hpp>
#include <dsac/concurrency/fibers/details/stack/stack.hpp>
#include <dsac/container/intrusive/list.hpp>
#include <dsac/functional/strong_type.hpp>

namespace dsac {

class fiber;

class fiber_scheduler final {
  friend class fiber;

public:
  using entry_routine = strong_type<fiber_routine, struct EntryRoutine>;
  using child_routine = strong_type<fiber_routine, struct ChildRoutine>;

  // Modifiers

  auto submit(entry_routine routine) -> void;
  auto submit(child_routine routine) -> void;

  // Observers

  static fiber_scheduler* current();

private:
  // Modifiers

  auto switch_to(fiber* fiber) -> void;

  auto dispatch_of(fiber* fiber) -> void;

  fiber*                 current_fiber_{};
  intrusive::list<fiber> fibers_{};
  execution_context      execution_context_{};
  fiber_stack_allocator  stacks_{};
};

}  // namespace dsac
