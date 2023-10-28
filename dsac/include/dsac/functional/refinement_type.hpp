#pragma once

#include <dsac/functional/strong_type.hpp>

namespace dsac {

template <typename T, typename Refinement>
concept Refinementable = requires(T a) {
  { Refinement{}(a) } -> std::same_as<bool>;
};

template <typename T, typename Parameter, Refinementable<T>... Refinement>
class refinement_type final : public strong_type<T, Parameter> {
  using base = strong_type<T, Parameter>;

public:
  explicit refinement_type(T value)
    : base(std::move(value)) {
    if (not(Refinement{}(base::get()), ...)) {
    }
  }
};

}  // namespace dsac
