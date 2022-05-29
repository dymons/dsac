#pragma once

#include "future.hpp"
#include "state.hpp"

namespace dsac {

class promise_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class promise_already_satisfied : public promise_exception {
public:
  promise_already_satisfied()
    : promise_exception("Promise already satisfied") {
  }
};

template <typename T>
class promise : public hold_state<T> {
  using hold_state<T>::state_;
  using hold_state<T>::get_state;
  using hold_state<T>::check_state;
  using hold_state<T>::release_state;

public:
  promise()
    : hold_state<T>(make_shared_state<T>()) {
  }

  promise(promise&& that) noexcept            = default;
  promise& operator=(promise&& that) noexcept = default;

  future<T> make_future() {
    assert(!std::exchange(future_extracted_, true));
    return future{state_};
  }

  void set(T value) {
    throw_if_fulfilled();
    release_state()->set_result(result<T>(std::move(value)));
  }

  void set(result<T>&& result) {
    throw_if_fulfilled();
    release_state()->set_result(std::move(result));
  }

  void set(std::exception_ptr&& exception) {
    release_state()->set_result(result<T>(std::move(exception)));
  }

private:
  void throw_if_fulfilled() const {
    if (get_state()->has_result()) {
      throw promise_already_satisfied{};
    }
  }

  bool future_extracted_{false};
};

}  // namespace dsac
