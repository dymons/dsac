#pragma once

#include "dsac/memory/shared_ptr.hpp"

#include "dsac/concurrency/fibers/details/routine/routine.hpp"

namespace dsac {

class fiber_scheduler final {
  // Constructors

  fiber_scheduler();

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
  class fiber_scheduler_pimpl;
  shared_ptr<fiber_scheduler_pimpl> pimpl_;
};

}  // namespace dsac
