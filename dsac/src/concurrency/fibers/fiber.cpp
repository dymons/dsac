#include <dsac/concurrency/fibers/fiber.hpp>

namespace dsac {

fiber::fiber(fiber_routine routine)
  : routine_(std::move(routine))
  , state_(fiber_state::starting) {
}

}  // namespace dsac