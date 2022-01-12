#pragma once

namespace algo::syncing::detail {
class NonMovable {
 public:
  NonMovable() = default;
  NonMovable(NonMovable&&) = delete;
  NonMovable& operator=(NonMovable&&) = delete;
};
}  // namespace algo::syncing::detail