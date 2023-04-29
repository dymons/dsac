#pragma once

#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/routine/routine.hpp>
#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/concurrency/fibers/stack/stack.hpp>
#include <dsac/concurrency/fibers/trampoline.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

using fiber_execution_context = execution_context;

enum class fiber_state : unsigned {
  starting,
  runnable,
  running,
  suspended,
  terminated,
};

class fiber final : public intrusive::list_node_base<fiber>, public trampoline_base {
  fiber_scheduler*        fiber_scheduler_{};
  fiber_routine           fiber_routine_{};
  fiber_state             fiber_state_{};
  fiber_stack             fiber_stack_{};
  fiber_execution_context fiber_execution_context_{};

  // Constructors

  explicit fiber(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine);

public:
  ~fiber() override = default;

  // Constructors

  [[nodiscard]] static fiber* make(fiber_scheduler* scheduler, fiber_stack stack, fiber_routine routine);

  // Observers

  [[nodiscard]] auto get_execution_context() & noexcept -> execution_context&;

  // Modifiers

  auto set_state(fiber_state state) noexcept -> void;

  void run() noexcept final;
};

}  // namespace dsac
