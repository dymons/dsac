#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

namespace dsac {
template <typename F>
auto AsyncVia(concurrency::IExecutorPtr executor, F routine) {
  using ReturnType = typename std::result_of<F()>::type;

  Promise<ReturnType> promise;
  Future<ReturnType>  future = promise.MakeFuture().Via(executor);
  executor->Submit(
      [promise = std::move(promise), routine = std::move(routine)]() mutable {
        promise.Set(routine());
      });

  return future;
}
}  // namespace dsac
