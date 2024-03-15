#pragma once

#include <utility>

namespace dsac {

template <typename T, typename Parameter>
class strong_type {
public:
  explicit strong_type(T value)
    : value_(std::move(value)) {
  }

  template <typename Self>
  constexpr decltype(auto) get(this Self&& self) {
    return std::forward<Self>(self).value_;
  }

  auto operator<=>(strong_type const&) const -> bool = default;

private:
  T value_;
};

}  // namespace dsac
