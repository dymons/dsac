#pragma once

#include <dsac/concurrency/fibers/details/routine/routine.hpp>

namespace dsac {

auto go(fiber_routine routine) -> void;

}  // namespace dsac
