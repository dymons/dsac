#ifndef CONCURRENCY_PROMISE_HPP
#error This file may only be included from promise.hpp
#endif

namespace dsac {

template <typename T>
future<T> promise<T>::make_future() {
  if (std::exchange(future_extracted_, true)) {
    throw future_already_exposed{};
  }
  return future{state_};
}

template <typename T>
void promise<T>::set(T value) {
  std::move(*this).release_state()->set_result(result<T>(std::move(value)));
}

template <typename T>
void promise<T>::set(result<T>&& result) {
  std::move(*this).release_state()->set_result(std::move(result));
}

template <typename T>
void promise<T>::set(std::exception_ptr&& exception) {
  std::move(*this).release_state()->set_result(result<T>(std::move(exception)));
}

}  // namespace dsac
