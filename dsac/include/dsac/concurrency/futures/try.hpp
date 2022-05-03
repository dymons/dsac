#pragma once

#include <exception>
#include <variant>

namespace dsac {
template <typename T>
class Try {
  std::variant<T, std::exception_ptr> store_;

public:
  explicit Try(T value)
    : store_(std::move(value)) {
  }

  explicit Try(std::exception_ptr&& exception)
    : store_(std::move(exception)) {
  }

  const T& ValueOrThrow() const {
    if (HasException()) {
      std::rethrow_exception(std::get<std::exception_ptr>(store_));
    }

    return std::get<T>(store_);
  }

  [[nodiscard]] bool HasValue() const noexcept {
    return store_.index() == 0;
  }

  [[nodiscard]] bool HasException() const noexcept {
    return store_.index() == 1;
  }
};

template<typename T>
using result = Try<T>;

template <typename T>
bool operator==(const Try<T>& p, const Try<T>& b) {
  return (p.HasValue() && b.HasValue()) &&
         (p.ValueOrThrow() == b.ValueOrThrow());
}
}  // namespace dsac