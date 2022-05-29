#pragma once

#include "future.hpp"
#include "state.hpp"

namespace dsac {

template <typename T>
class promise final : public hold_state<T> {
  using hold_state<T>::state_;
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
    release_state()->set_result(result<T>(std::move(value)));
  }

  void set(result<T>&& result) {
    release_state()->set_result(std::move(result));
  }

  void set(std::exception_ptr&& exception) {
    release_state()->set_result(result<T>(std::move(exception)));
  }

private:
  bool future_extracted_{false};
};

}  // namespace dsac
