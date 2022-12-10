#pragma once

#include <optional>

namespace dsac {

struct request final {
  std::optional<int>         value;
  std::optional<std::size_t> timestamp;
};

}  // namespace dsac