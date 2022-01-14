#pragma once

namespace algo::detail {
class NonMovable {
 public:
  NonMovable() = default;
  NonMovable(NonMovable&&) = delete;
  NonMovable& operator=(NonMovable&&) = delete;
};
}  // namespace algo::detail