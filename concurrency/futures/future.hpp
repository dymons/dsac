#pragma once

#include <concurrency/futures/state.hpp>

namespace algo::futures {

template <typename T>
class Promise;

template <typename T>
class Future : public HoldState<T> {
  friend class Promise<T>;

  using HoldState<T>::state_;
  using HoldState<T>::CheckState;
  using HoldState<T>::ReleaseState;

 public:
  Try<T> GetResult() && {
    return ReleaseState()->GetResult();
  }

 private:
  explicit Future(StateRef<T> state) : HoldState<T>(std::move(state)) {
  }
};
}  // namespace algo::futures