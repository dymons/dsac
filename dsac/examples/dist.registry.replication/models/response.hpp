#pragma once

#include <optional>
#include <string>

namespace dsac {

struct response final {
  std::optional<int>         value;
  std::optional<std::size_t> timestamp;
};

}  // namespace dsac