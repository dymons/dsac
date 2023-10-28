#pragma once

#include <dsac/functional/strong_type.hpp>

namespace dsac {

template <typename T, typename Parameter, typename... Refinement>
class refinement_type final : public strong_type<T, Parameter> {
  using base = strong_type<T, Parameter>;

public:
  explicit refinement_type(T value)
    : base(std::move(value)) {
    (Refinement{}(base::get()), ...);
  }
};

}  // namespace dsac
