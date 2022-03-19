#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

#include <optional>
#include <coroutine>

namespace algo::coroutines {
template <typename T>
class FutureAwaiter {
 public:
  FutureAwaiter(futures::Future<T>&& future) : future_(std::move(future)) {
  }

  bool await_ready() {
    if (result_.has_value()) {
    }
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) {
    std::move(future_).Subscribe([this, h](futures::Try<T> result) mutable {
      result_.emplace(std::move(result));
      h.resume();
    });
  }

  auto await_resume() {
    return (*result_).ValueOrThrow();
  }

 private:
  futures::Future<T> future_;
  std::optional<futures::Try<T>> result_;
};
}  // namespace algo::coroutines

template <typename T>
auto operator co_await(algo::futures::Future<T>&& future) {
  return algo::coroutines::FutureAwaiter<T>(std::move(future));
}