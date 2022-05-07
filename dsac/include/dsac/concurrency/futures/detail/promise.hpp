#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/state.hpp>

namespace dsac {

class PromiseException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class PromiseAlreadySatisfied : public PromiseException {
public:
  PromiseAlreadySatisfied()
    : PromiseException("Promise already satisfied") {
  }
};

}  // namespace dsac

//////////////////////////////////////////////////////////////////////

namespace dsac::detail {
template <typename T>
class Promise : public HoldState<T> {
  using HoldState<T>::state_;
  using HoldState<T>::GetState;
  using HoldState<T>::CheckState;
  using HoldState<T>::ReleaseState;

public:
  Promise()
    : HoldState<T>(MakeSharedState<T>()) {
  }

  Promise(Promise&& that)            = default;
  Promise& operator=(Promise&& that) = default;

  Future<T> MakeFuture() {
    assert(!std::exchange(future_extracted_, true));
    return Future{state_};
  }

  void Set(T value) {
    ThrowIfFulfilled();
    ReleaseState()->SetResult(Try<T>(std::move(value)));
  }

  void Set(Try<T>&& value) {
    ThrowIfFulfilled();
    ReleaseState()->SetResult(std::move(value));
  }

  void Set(std::exception_ptr&& exception) {
    ReleaseState()->SetResult(Try<T>(std::move(exception)));
  }

private:
  void ThrowIfFulfilled() const {
    if (GetState()->HasResult()) {
      throw PromiseAlreadySatisfied{};
    }
  }

  bool future_extracted_{false};
};
}  // namespace dsac::detail