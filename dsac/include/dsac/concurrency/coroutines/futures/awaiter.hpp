#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <coroutine>
#include <optional>

namespace dsac {

template <typename T>
class future_awaiter {
public:
  explicit future_awaiter(future<T>&& future)
    : future_(std::move(future)) {
  }

  auto await_ready() -> bool {
    if (result_.has_value()) {
    }
    return false;
  }

  auto await_suspend(std::coroutine_handle<> h) -> void {
    std::move(future_).subscribe([this, h](result<T> result) mutable {
      result_.emplace(std::move(result));
      h.resume();
    });
  }

  auto await_resume() {
    return result_->value_or_throw();
  }

private:
  future<T>                future_;
  std::optional<result<T>> result_;
};

}  // namespace dsac

template <typename T>
auto operator co_await(dsac::future<T>&& future) {
  return dsac::future_awaiter<T>(std::move(future));
}