#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <coroutine>

namespace dsac {
template <typename T>
struct CoroutinePromise {
  promise<T> promise_;

  auto get_return_object() {
    return promise_.make_future();
  }

  std::suspend_never initial_suspend() noexcept {
    return {};
  }

  std::suspend_never final_suspend() noexcept {
    return {};
  }

  void set_exception(std::exception_ptr e) {
    std::move(promise_).set(std::move(e));
  }

  void unhandled_exception() {
    std::move(promise_).set(std::current_exception());
  }

  template <typename U>
  void return_value(U&& u) {
    std::move(promise_).set(result<U>(std::forward<U>(u)));
  }
};

}  // namespace dsac

template <typename R, typename... Args>
struct std::coroutine_traits<dsac::future<R>, Args...> {
  using promise_type = dsac::CoroutinePromise<R>;
};
