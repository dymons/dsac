#pragma once

#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/routine/routine.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

enum class fiber_state : unsigned {
  starting,
  runnable,
  running,
  suspended,
  terminated,
};

class fiber : public intrusive::list_node_base<fiber> {
  fiber_routine     fiber_routine_;
  fiber_state       fiber_state_;
  execution_context fiber_execution_context_;

  explicit fiber(fiber_routine routine);

public:
  [[nodiscard]] static fiber* make(fiber_routine routine);

  auto set_state(fiber_state state) noexcept -> void;

  auto get_execution_context() & noexcept -> execution_context&;
};

}  // namespace dsac
