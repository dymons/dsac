#pragma once

#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/concurrency/fibers/routine.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

class fiber_scheduler final {
public:
  // Modifiers

  auto main(fiber_routine entry_routine) & -> void;

  auto submit(fiber_routine routine) -> fiber_scheduler*;

private:
  auto execute() -> void;

  intrusive::list<fiber> fibers_;
};

}  // namespace dsac
