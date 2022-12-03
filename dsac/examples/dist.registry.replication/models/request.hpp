#pragma once

#include <chrono>
#include <optional>
#include <string>

namespace dsac {

struct request final {
  std::optional<int>                                                value;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> timestamp;
};

}  // namespace dsac