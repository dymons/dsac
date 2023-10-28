#pragma once

#include <dsac/functional/strong_type.hpp>

namespace dsac {

template <typename T, typename Parameter>
class refinement_type : public strong_type<T, Parameter> {
public:
  explicit refinement_type(T value)
    : strong_type<T, Parameter>(std::move(value)) {
  }
};

}  // namespace dsac
