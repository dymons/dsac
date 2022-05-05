#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

#include <atomic>

namespace dsac {
template <typename T>
future<T> first_of(std::vector<future<T>>&& futures) {
  struct Context {
    promise<T>       promise_;
    std::atomic_bool done_{false};
  };

  auto ctx = std::make_shared<Context>();
  for (future<T>& future : futures) {
    std::move(future).Subscribe([ctx](Try<T>&& t) {
      if (!ctx->done_.exchange(true, std::memory_order_relaxed)) {
        ctx->promise_.Set(std::move(t));
      }
    });
  }

  return ctx->promise_.MakeFuture();
}
}  // namespace dsac