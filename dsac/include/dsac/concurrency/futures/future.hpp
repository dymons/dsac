#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/callback.hpp>
#include <dsac/concurrency/futures/state.hpp>

namespace dsac {

class FutureException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class FutureNoExecutor : public FutureException {
public:
  FutureNoExecutor()
    : FutureException("No executor provided to via") {
  }
};

class FutureNoCallback : public FutureException {
public:
  FutureNoCallback()
    : FutureException("No callback provided to subscribe") {
  }
};

//////////////////////////////////////////////////////////////////////

template <typename T>
class Promise;

//////////////////////////////////////////////////////////////////////

template <typename T>
class Future : public HoldState<T> {
  friend class Promise<T>;

  using HoldState<T>::state_;
  using HoldState<T>::GetState;
  using HoldState<T>::CheckState;
  using HoldState<T>::ReleaseState;

public:
  static Future<T> Fail(const char* message) {
    auto state = MakeSharedState<T>();
    state->SetResult(
        Try<T>{std::make_exception_ptr(std::logic_error{message})});
    return Future<T>(std::move(state));
  }

  Try<T> Get() && {
    return ReleaseState()->GetResult();
  }

  Future<T> Via(concurrency::IExecutorPtr exec) && {
    if (exec == nullptr) {
      throw FutureNoExecutor{};
    }

    Future<T> new_future = Future<T>(ReleaseState());
    new_future.SetExecutor(std::move(exec));
    return new_future;
  }

  Future<T> Subscribe(callback<T> callback) && {
    if (callback == nullptr) {
      throw FutureNoCallback{};
    }

    Future<T> new_future = Future<T>(ReleaseState());
    new_future.SetCallback(std::move(callback));
    return new_future;
  }

  template <typename F>
  auto Then(F&& continuation) && {
    using ReturnType = typename std::result_of<F(const Try<T>&)>::type;

    Promise<ReturnType> promise;
    Future<ReturnType>  future = promise.MakeFuture();

    std::move(*this).Subscribe(
        [continuation = std::forward<F>(continuation),
         promise      = std::move(promise)](Try<T> result) mutable {
          try {
            promise.Set(continuation(result));
          } catch (...) {
            promise.Set(std::current_exception());
          }
        });

    return future;
  }

private:
  void SetExecutor(concurrency::IExecutorPtr&& exec) {
    GetState()->SetExecutor(std::move(exec));
  }

  void SetCallback(callback<T>&& callback) {
    GetState()->SetCallback(std::move(callback));
  }

  explicit Future(StateRef<T> state)
    : HoldState<T>(std::move(state)) {
  }
};
}  // namespace dsac