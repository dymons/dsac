#pragma once

#include <dsac/concurrency/fibers/routine.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

enum class fiber_state : unsigned {
  starting,
  runnable,
  running,
  suspended,
  terminated,
};

class fiber final : public intrusive::list_node_base<fiber> {
  fiber_routine routine_;
  fiber_state   state_;

public:
  explicit fiber(fiber_routine routine);
};

}  // namespace dsac
