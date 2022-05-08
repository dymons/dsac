#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

namespace dsac {
template <typename F>
auto async_via(executor_base_ref executor, F routine) {
  using return_type = typename std::result_of<F()>::type;

  promise<return_type> promise;
  future<return_type>  future = promise.make_future().via(executor);
  executor->submit([promise = std::move(promise), routine = std::move(routine)]() mutable { promise.set(routine()); });

  return future;
}
}  // namespace dsac
