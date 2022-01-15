#pragma once

#include <concurrency/futures/try.hpp>
#include <concurrency/syncing/mvar.hpp>
#include <concurrency/executors/executor.hpp>

namespace algo::futures {
template <typename T>
class SharedState {
  syncing::MVar<Try<T>> storage_;
  concurrency::IExecutorPtr executor_;

 public:
  SharedState() {
  }

  void SetResult(Try<T>&& result) {
    storage_.Put(std::move(result));
  }

  bool HasResult() {
    return !storage_.IsEmpty();
  }

  Try<T> GetResult() {
    return storage_.Take();
  }

  void SetExecutor(concurrency::IExecutorPtr&& exec) {
    executor_ = std::move(exec);
  }
};

template <typename T>
using StateRef = std::shared_ptr<SharedState<T>>;

template <typename T>
inline StateRef<T> MakeSharedState() {
  return std::make_shared<SharedState<T>>();
}

//////////////////////////////////////////////////////////////////////

template <typename T>
class HoldState {
 protected:
  HoldState(StateRef<T> state) : state_(std::move(state)) {
  }

  HoldState(HoldState&& that) = default;
  HoldState& operator=(HoldState&& that) = default;

  HoldState(const HoldState& that) = delete;
  HoldState& operator=(const HoldState& that) = delete;

  StateRef<T> ReleaseState() {
    CheckState();
    return std::move(state_);
  }

  StateRef<T> const& GetState() const {
    CheckState();
    return state_;
  }

  bool HasState() const {
    return (bool)state_;
  }

  void CheckState() const {
    assert(HasState());
  }

 protected:
  StateRef<T> state_;
};
}  // namespace algo::futures