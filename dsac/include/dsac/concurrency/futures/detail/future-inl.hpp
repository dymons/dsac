#ifndef CONCURRENCY_FUTURE_HPP
#error This file may only be included from future.hpp
#endif

namespace dsac {

template <typename T>
result<T> future<T>::get() && noexcept {
  return std::move(*this).release_state()->get_result();
}

template <typename T>
future<T> future<T>::via(executor_base_ref exec) && {
  if (exec == nullptr) {
    throw future_no_executor{};
  }

  auto state = std::move(*this).release_state();
  state->set_executor(std::move(exec));
  return future<T>(std::move(state));
}

template <typename T>
future<T> future<T>::subscribe(callback<T> callback) && {
  if (callback == nullptr) {
    throw future_no_callback{};
  }

  auto state = std::move(*this).release_state();
  state->set_callback(std::move(callback));
  return future<T>(std::move(state));
}

template <typename T>
template <typename F>
auto future<T>::then(F&& continuation) && {
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

template <typename T>
future<T>::future(state_ref<T> state)
  : hold_state<T>(std::move(state)) {
}

template <typename T>
future<T> make_future_on_error(const char* message) {
  auto state = make_shared_state<T>();
  state->set_result(result<T>{std::make_exception_ptr(std::logic_error{message})});
  return future<T>(std::move(state));
}

} // namespace dsac
