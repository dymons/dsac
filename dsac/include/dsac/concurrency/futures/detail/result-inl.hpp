#ifndef CONCURRENCY_RESULT_HPP
#error This file may only be included from result.hpp
#endif

namespace dsac {

template <typename T>
result<T>::result(T value)
  : store_(std::move(value)) {
}

template <typename T>
result<T>::result(std::exception_ptr&& exception)
  : store_(std::move(exception)) {
}

template <typename T>
const T& result<T>::value_or_throw() const& {
  if (has_exception()) [[unlikely]] {
    std::rethrow_exception(std::get<std::exception_ptr>(store_));
  }

  return std::get<T>(store_);
}

template <typename T>
auto result<T>::value_or_throw() & -> T& {
  if (has_exception()) [[unlikely]] {
    std::rethrow_exception(std::get<std::exception_ptr>(store_));
  }

  return std::get<T>(store_);
}

template <typename T>
auto result<T>::value_or_throw() && -> T&& {
  if (has_exception()) [[unlikely]] {
    std::rethrow_exception(std::get<std::exception_ptr>(store_));
  }

  return std::get<T>(std::move(store_));
}

template <typename T>
auto result<T>::value_or_throw() const&& -> const T&& {
  if (has_exception()) [[unlikely]] {
    std::rethrow_exception(std::get<std::exception_ptr>(store_));
  }

  return std::get<T>(std::move(store_));
}

template <typename T>
bool result<T>::has_value() const noexcept {
  return store_.index() == 0;
}

template <typename T>
bool result<T>::has_exception() const noexcept {
  return store_.index() == 1;
}

}  // namespace dsac
