#pragma once

#include <exception>
#include <variant>

namespace dsac {
template <typename T>
class result {
  std::variant<T, std::exception_ptr> store_;

public:
  explicit result(T value)
    : store_(std::move(value)) {
  }

  explicit result(std::exception_ptr&& exception)
    : store_(std::move(exception)) {
  }

  const T& value_or_throw() const {
    if (has_exception()) {
      std::rethrow_exception(std::get<std::exception_ptr>(store_));
    }

    return std::get<T>(store_);
  }

  [[nodiscard]] bool has_value() const noexcept {
    return store_.index() == 0;
  }

  [[nodiscard]] bool has_exception() const noexcept {
    return store_.index() == 1;
  }
};

template <typename T>
bool operator==(const result<T>& p, const result<T>& b) {
  return (p.has_value() && b.has_value()) &&
         (p.value_or_throw() == b.value_or_throw());
}
}  // namespace dsac