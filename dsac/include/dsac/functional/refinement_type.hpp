#pragma once

#include <dsac/functional/strong_type.hpp>

namespace dsac {

class runtime_refinement_error final : public std::exception {
public:
  virtual const char *what() const noexcept override {
    return "Runtime refinement type check";
  }
};

template <typename T, typename Parameter, typename... Refinement>
class refinement_type final : public strong_type<T, Parameter> {
  using base = strong_type<T, Parameter>;

public:
  explicit refinement_type(T value)
    : base(std::move(value)) {
    if (not(Refinement{}(base::get()), ...)) {
      throw runtime_refinement_error{};
    }
  }
};

}  // namespace dsac
