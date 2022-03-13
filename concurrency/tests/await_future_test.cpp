#include <catch2/catch.hpp>

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>
#include <concurrency/futures/combine/first_of.hpp>
#include <concurrency/executors/static_thread_pool.hpp>

#include <optional>
#include <coroutine>

using namespace algo::futures;
using namespace algo::concurrency;

template <typename F>
auto AsyncVia(IExecutorPtr executor, F routine) {
  using ReturnType = typename std::result_of<F()>::type;

  Promise<ReturnType> promise;
  Future<ReturnType> future = promise.MakeFuture().Via(executor);
  executor->Submit([promise = std::move(promise), routine]() mutable {
    promise.Set(routine());
  });

  return future;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct CoroutinePromise {
  Promise<T> promise_;

  auto get_return_object() {
    return promise_.MakeFuture();
  }

  std::suspend_never initial_suspend() noexcept {
    return {};
  }

  std::suspend_never final_suspend() noexcept {
    return {};
  }

  void set_exception(std::exception_ptr e) {
    std::move(promise_).Set(std::move(e));
  }

  void unhandled_exception() {
    std::move(promise_).Set(std::current_exception());
  }

  template <typename U>
  void return_value(U&& u) {
    std::move(promise_).Set(Try<U>(std::forward<U>(u)));
  }
};

template <typename R, typename... Args>
struct std::coroutine_traits<Future<R>, Args...> {
  using promise_type = CoroutinePromise<R>;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class FutureAwaiter {
 public:
  FutureAwaiter(Future<T>&& future) : future_(std::move(future)) {
  }

  bool await_ready() {
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) {
    std::move(future_).Subscribe([this, h](Try<T> result) mutable {
      result_.emplace(std::move(result));
      h.resume();
    });
  }

  auto await_resume() {
    return (*result_).ValueOrThrow();
  }

 private:
  Future<T> future_;
  std::optional<Try<T>> result_;
};

template <typename T>
auto operator co_await(Future<T>&& future) {
  return FutureAwaiter<T>(std::move(future));
}

//////////////////////////////////////////////////////////////////////////////////////////

Future<int> Coroutine(IExecutorPtr executor) {
  Future<int> future = AsyncVia(executor, []() {
    return 42;
  });
  co_return co_await std::move(future);
}

TEST_CASE("Stackless coroutine C++", "[awaiter]") {
  constexpr const std::size_t kNumberWorkers = 4U;
  auto executor(StaticThreadPool::Make(kNumberWorkers));

  auto future = Coroutine(executor);
  REQUIRE(std::move(future).Get().ValueOrThrow() == 42);

  executor->Join();
}