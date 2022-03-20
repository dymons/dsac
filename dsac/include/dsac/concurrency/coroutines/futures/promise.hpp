#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

#include <coroutine>

namespace dsac::coroutines {
template <typename T>
struct CoroutinePromise {
  futures::Promise<T> promise_;

  auto get_return_object() {
    return promise_.MakeFuture();
  }

  std::suspend_never initial_suspend() noexcept {
    return {};
  }

  std::suspend_never final_suspend() noexcept {
    return {};
  }

  void set_exception(std::exception_ptr e) {
    std::move(promise_).Set(std::move(e));
  }

  void unhandled_exception() {
    std::move(promise_).Set(std::current_exception());
  }

  template <typename U>
  void return_value(U&& u) {
    std::move(promise_).Set(futures::Try<U>(std::forward<U>(u)));
  }
};

}  // namespace dsac::coroutines

template <typename R, typename... Args>
struct std::coroutine_traits<dsac::futures::Future<R>, Args...> {
  using promise_type = dsac::coroutines::CoroutinePromise<R>;
};