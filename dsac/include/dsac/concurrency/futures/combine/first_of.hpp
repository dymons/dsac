#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

#include <atomic>

namespace dsac::futures {
template <typename T>
Future<T> FirstOf(std::vector<Future<T>>&& futures)
{
  struct Context {
    Promise<T>       promise;
    std::atomic_bool done{false};
  };

  auto ctx = std::make_shared<Context>();
  for (Future<T>& future : futures) {
    std::move(future).Subscribe([ctx](Try<T>&& t) {
      if (!ctx->done.exchange(true, std::memory_order_relaxed)) {
        ctx->promise.Set(std::move(t));
      }
    });
  }

  return ctx->promise.MakeFuture();
}
}  // namespace dsac::futures