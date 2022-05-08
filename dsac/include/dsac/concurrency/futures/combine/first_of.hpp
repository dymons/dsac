#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <atomic>

namespace dsac {
template <typename T>
future<T> first_of(std::vector<future<T>>&& futures) {
  struct context {
    promise<T>       promise_;
    std::atomic_bool done_{false};
  };

  auto ctx = std::make_shared<context>();
  for (future<T>& future : futures) {
    std::move(future).subscribe([ctx](result<T>&& result) {
      if (!ctx->done_.exchange(true, std::memory_order_relaxed)) {
        ctx->promise_.set(std::move(result));
      }
    });
  }

  return ctx->promise_.make_future();
}
}  // namespace dsac