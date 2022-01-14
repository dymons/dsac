#pragma once

#include <concurrency/futures/try.hpp>
#include <concurrency/syncing/mvar.hpp>

namespace algo::futures {
template <typename T>
class SharedState {
  syncing::MVar<Try<T>> storage_;

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
};

template <typename T>
using StateRef = std::shared_ptr<SharedState<T>>;

template <typename T>
inline StateRef<T> MakeSharedState() {
  return std::make_shared<SharedState<T>>();
}

template <typename T>
class HoldState : public detail::NonCopyable {
 protected:
  HoldState(StateRef<T> state) : state_(std::move(state)) {
  }

  // Movable
  HoldState(HoldState&& that) = default;
  HoldState& operator=(HoldState&& that) = default;

  StateRef<T> ReleaseState() {
    CheckState();
    return std::move(state_);
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