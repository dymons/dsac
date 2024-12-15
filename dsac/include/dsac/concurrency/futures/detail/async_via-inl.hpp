#ifndef CONCURRENCY_ASYNC_VIA_HPP
#error This file may only be included from async_via.hpp
#endif

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>

namespace dsac {

template <typename F>
auto async_via(shared_ptr<iexecutor> executor, F&& routine) {
  using return_type = typename std::result_of_t<F()>;

  promise<return_type> promise;
  future<return_type>  future = promise.make_future().via(executor);
  executor->submit([promise = std::move(promise), routine = std::forward<F>(routine)]() mutable {
    try {
      promise.set(routine());
    } catch (...) {
      promise.set(std::current_exception());
    }
  });

  return future;
}

}  // namespace dsac
