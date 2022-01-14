#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/state.hpp>

namespace algo::futures {
template <typename T>
class Promise : public HoldState<T> {
  using HoldState<T>::state_;
  using HoldState<T>::CheckState;
  using HoldState<T>::ReleaseState;

 public:
  Promise() : HoldState<T>(MakeSharedState<T>()) {
  }

  Future<T> MakeFuture() {
    assert(!std::exchange(future_extracted_, true));
    return Future{state_};
  }

  void SetValue(T value) && {
    ReleaseState()->SetResult(Try<T>(std::move(value)));
  }

 private:
  bool future_extracted_{false};
};
}  // namespace algo::futures