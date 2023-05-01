#pragma once

#include <utility>

namespace dsac {

static auto kNop = [] {};

template <typename C, typename D>
[[nodiscard("")]] auto defer(C c, D d) {
  class scope {
    C c_;
    D d_;

  public:
    inline explicit scope(C c, D d)
      : c_(std::move(c))
      , d_(std::move(d)) {
      c_();
    }
    inline ~scope() {
      d_();
    }
  };

  return scope{std::move(c), std::move(d)};
}

template <typename D>
[[nodiscard("")]] auto defer(D d) {
  return defer(kNop, std::move(d));
}

}  // namespace dsac
