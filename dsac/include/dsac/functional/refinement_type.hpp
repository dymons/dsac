#pragma once

#include <dsac/functional/strong_type.hpp>

namespace dsac {

class runtime_refinement_error final : public std::exception {
public:
  auto what() const noexcept -> const char* final {
    return "Runtime refinement type check";
  }
};

template <typename T, typename Parameter, typename... Refinement>
class refinement_type final : public strong_type<T, Parameter> {
  using base = strong_type<T, Parameter>;

public:
  explicit refinement_type(T value)
    : base(std::move(value)) {
    const auto is_refinementable = (... && Refinement{}(base::get()));
    if (not is_refinementable) {
      throw runtime_refinement_error{};
    }
  }
};

}  // namespace dsac
