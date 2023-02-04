#pragma once

#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

class fiber_scheduler final {
private:
  intrusive::list<fiber> fibers_;
};

}  // namespace dsac
