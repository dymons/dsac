#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/callback.hpp>
#include <dsac/concurrency/futures/state.hpp>

namespace dsac {

class future_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class future_no_executor : public future_exception {
public:
  future_no_executor()
    : future_exception("No executor provided to via") {
  }
};

class future_no_callback : public future_exception {
public:
  future_no_callback()
    : future_exception("No callback provided to subscribe") {
  }
};

template <typename T>
class promise;

template <typename T>
class future : public hold_state<T> {
  friend class promise<T>;

  template <typename U>
  friend future<U> make_future_on_error(const char* message);

  using hold_state<T>::get_state;
  using hold_state<T>::check_state;
  using hold_state<T>::release_state;

public:
  result<T> get() && {
    return release_state()->get_result();
  }

  future<T> via(executor_base_ref exec) && {
    if (exec == nullptr) {
      throw future_no_executor{};
    }

    future<T> new_future = future<T>(release_state());
    new_future.set_executor(std::move(exec));
    return new_future;
  }

  future<T> subscribe(callback<T> callback) && {
    if (callback == nullptr) {
      throw future_no_callback{};
    }

    future<T> new_future = future<T>(release_state());
    new_future.set_callback(std::move(callback));
    return new_future;
  }

  template <typename F>
  auto then(F&& continuation) && {
    using return_type = typename std::result_of<F(const result<T>&)>::type;

    promise<return_type> promise;
    future<return_type>  future = promise.make_future();

    std::move(*this).subscribe(
        [continuation = std::forward<F>(continuation), promise = std::move(promise)](result<T> result) mutable {
          try {
            promise.set(continuation(result));
          } catch (...) {
            promise.set(std::current_exception());
          }
        });

    return future;
  }

private:
  void set_executor(executor_base_ref&& exec) {
    get_state()->set_executor(std::move(exec));
  }

  void set_callback(callback<T>&& callback) {
    get_state()->set_callback(std::move(callback));
  }

  explicit future(state_ref<T> state)
    : hold_state<T>(std::move(state)) {
  }
};

template <typename T>
[[nodiscard]] future<T> make_future_on_error(const char* message) {
  auto state = make_shared_state<T>();
  state->set_result(result<T>{std::make_exception_ptr(std::logic_error{message})});
  return future<T>(std::move(state));
}

}  // namespace dsac
