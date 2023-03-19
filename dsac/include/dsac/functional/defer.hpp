#pragma once

#include <utility>

namespace dsac {

template <typename F>
[[nodiscard("")]] auto defer(F f) {
  class [[maybe_unused]] scope {
    F f;

  public:
    inline explicit scope(F f)
      : f(std::move(f)) {
    }
    inline ~scope() {
      f();
    }
  };

  return scope{std::move(f)};
}

}  // namespace dsac
