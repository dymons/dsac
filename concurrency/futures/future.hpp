#pragma once

#include <concurrency/futures/state.hpp>
#include <concurrency/futures/callback.hpp>
#include <concurrency/executors/executor.hpp>

namespace algo::futures {

class FutureException : public std::logic_error {
 public:
  using std::logic_error::logic_error;
};

class FutureNoExecutor : public FutureException {
 public:
  FutureNoExecutor() : FutureException("No executor provided to via") {
  }
};

class FutureNoCallback : public FutureException {
 public:
  FutureNoCallback() : FutureException("No callback provided to subscribe") {
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

  Future<T> Subscribe(Callback<T> callback) && {
    if (callback == nullptr) {
      throw FutureNoCallback{};
    }

    Future<T> new_future = Future<T>(ReleaseState());
    new_future.SetCallback(std::move(callback));
    return new_future;
  }

 private:
  void SetExecutor(concurrency::IExecutorPtr&& exec) {
    GetState()->SetExecutor(std::move(exec));
  }

  void SetCallback(Callback<T>&& callback) {
    GetState()->SetCallback(std::move(callback));
  }

  explicit Future(StateRef<T> state) : HoldState<T>(std::move(state)) {
  }
};
}  // namespace algo::futures