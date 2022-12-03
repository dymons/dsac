#pragma once

#include <chrono>
#include <optional>
#include <string>

namespace dsac {

struct response final {
  std::optional<std::string>                                        value;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> timestamp;
};

}  // namespace dsac