#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>
#include <concurrency/executors/executor.hpp>

namespace dsac::futures {
template <typename F>
auto AsyncVia(concurrency::IExecutorPtr executor, F routine) {
  using ReturnType = typename std::result_of<F()>::type;

  Promise<ReturnType> promise;
  Future<ReturnType> future = promise.MakeFuture().Via(executor);
  executor->Submit(
      [promise = std::move(promise), routine = std::move(routine)]() mutable {
        promise.Set(routine());
      });

  return future;
}
}  // namespace dsac::futures
