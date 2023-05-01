#pragma once

#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/routine/routine.hpp>
#include <dsac/concurrency/fibers/details/stack/stack.hpp>
#include <dsac/container/intrusive/list.hpp>
#include <dsac/concurrency/fibers/details/trampoline.hpp>

namespace dsac {

class fiber_scheduler;

class [[nodiscard]] fiber final : public intrusive::list_node_base<fiber>, public trampoline_base {
  fiber_scheduler*  fiber_scheduler_{};
  fiber_routine     fiber_routine_{};
  fiber_stack       fiber_stack_{};
  execution_context fiber_execution_context_{};

  // Constructors

  explicit fiber(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine);

public:
  // Constructors

  [[nodiscard]] static fiber* make(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine);

  // Destructor

  ~fiber() final = default;

  // Observers

  [[nodiscard]] auto get_execution_context() & noexcept -> execution_context&;
  [[nodiscard]] auto get_stack() & noexcept -> fiber_stack&;

  // Modifiers

  void run() noexcept final;
};

}  // namespace dsac
