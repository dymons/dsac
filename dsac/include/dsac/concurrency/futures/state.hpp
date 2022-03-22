#pragma once

#include <concurrency/executors/executor.hpp>
#include <concurrency/futures/callback.hpp>
#include <concurrency/futures/try.hpp>
#include <concurrency/syncing/mvar.hpp>

#include <variant>

// TODO: Не эффективная реализация SharedState, посмотреть и изучить реализацию из
//       библиотеки folly, основанную на использовании FSM

namespace dsac::futures {
template <typename T>
class SharedState {
  template <typename U>
  using MVarRef    = std::shared_ptr<syncing::MVar<U>>;
  using StateValue = std::variant<Try<T>, Callback<T>>;

  MVarRef<StateValue>                      storage_;
  syncing::MVar<void>                      has_value_;
  syncing::MVar<concurrency::IExecutorPtr> executor_;

public:
  SharedState()
    : storage_(std::make_shared<syncing::MVar<StateValue>>())
  {
  }

  /// Call only from producer thread
  void SetResult(Try<T>&& result)
  {
    if (HasResult()) {
      // If we already have the value, what to do? ¯\_(ツ)_/¯
    }
    else if (HasCallback()) {
      Callback<T> callback = std::get<Callback<T>>(storage_->Take());
      storage_->Put(std::move(result));
      has_value_.Put();
      DoCallback(std::move(callback));
    }
    else {
      storage_->Put(std::move(result));
      has_value_.Put();
    }
  }

  [[nodiscard]] bool HasResult() const
  {
    bool const not_empty = !storage_->IsEmpty();
    bool const has_value = not_empty && storage_->ReadOnly().index() == 0;
    return has_value;
  }

  [[nodiscard]] bool HasCallback() const
  {
    bool const not_empty    = !storage_->IsEmpty();
    bool const has_callback = not_empty && storage_->ReadOnly().index() == 1;
    return has_callback;
  }

  Try<T> GetResult()
  {
    // We are waiting for the user value to be saved in the storage
    has_value_.ReadOnly();

    // Then we try to get this value from the storage
    return std::get<Try<T>>(storage_->ReadOnly());
  }

  /// Call only from consumer thread
  void SetExecutor(concurrency::IExecutorPtr&& exec)
  {
    assert(executor_.IsEmpty());
    executor_.Put(std::move(exec));
  }

  /// May call from any thread
  concurrency::IExecutorPtr GetExecutor()
  {
    return executor_.IsEmpty() ? nullptr : executor_.ReadOnly();
  }

  void SetCallback(Callback<T>&& callback)
  {
    if (HasCallback()) {
      // If we already have the callback, what to do? ¯\_(ツ)_/¯
    }
    else if (HasResult()) {
      DoCallback(std::move(callback));
    }
    else {
      storage_->Put(std::move(callback));
    }
  }

private:
  void DoCallback(Callback<T>&& callback)
  {
    if (concurrency::IExecutorPtr executor = GetExecutor(); executor != nullptr) {
      executor->Submit([callback = std::move(callback), storage = storage_]() mutable {
        callback(std::get<Try<T>>(storage->ReadOnly()));
      });
    }
    else {
      callback(std::get<Try<T>>(storage_->ReadOnly()));
    }
  }
};

template <typename T>
using StateRef = std::shared_ptr<SharedState<T>>;

template <typename T>
inline StateRef<T> MakeSharedState()
{
  return std::make_shared<SharedState<T>>();
}

//////////////////////////////////////////////////////////////////////

template <typename T>
class HoldState : public detail::NonCopyable {
protected:
  HoldState(StateRef<T> state)
    : state_(std::move(state))
  {
  }

  HoldState(HoldState&& that) = default;
  HoldState& operator=(HoldState&& that) = default;

  StateRef<T> ReleaseState()
  {
    CheckState();
    return std::move(state_);
  }

  StateRef<T> const& GetState() const
  {
    CheckState();
    return state_;
  }

  bool HasState() const
  {
    return (bool)state_;
  }

  void CheckState() const
  {
    assert(HasState());
  }

protected:
  StateRef<T> state_;
};
}  // namespace dsac::futures