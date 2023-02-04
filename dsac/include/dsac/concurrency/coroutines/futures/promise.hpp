#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <coroutine>

namespace dsac {

template <typename T>
class coroutine_promise {
  promise<T> promise_;

public:
  auto get_return_object() -> future<T> {
    return promise_.make_future();
  }

  auto initial_suspend() noexcept -> std::suspend_never {
    return {};
  }

  auto final_suspend() noexcept -> std::suspend_never {
    return {};
  }

  auto set_exception(std::exception_ptr e) -> void {
    std::move(promise_).set(std::move(e));
  }

  auto unhandled_exception() -> void {
    std::move(promise_).set(std::current_exception());
  }

  template <typename U>
  auto return_value(U&& u) -> void {
    std::move(promise_).set(result<U>(std::forward<U>(u)));
  }
};

}  // namespace dsac

template <typename R, typename... Args>
struct std::coroutine_traits<dsac::future<R>, Args...> {
  using promise_type = dsac::coroutine_promise<R>;
};
