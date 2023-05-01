#pragma once

#include <utility>

namespace dsac {

template <typename T, typename Parameter>
class strong_type {
public:
  explicit strong_type(T value)
    : value_(std::move(value)) {
  }
  auto get() const& noexcept -> T const& {
    return value_;
  }

  auto get() && -> T {
    return std::move(value_);
  }

private:
  T value_;
};

}  // namespace dsac
