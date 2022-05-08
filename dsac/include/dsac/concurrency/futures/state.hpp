#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/callback.hpp>
#include <dsac/concurrency/futures/try.hpp>
#include <dsac/concurrency/synchronization/mvar.hpp>

#include <variant>

namespace dsac {

template <typename T>
class shared_state {
  template <typename U>
  using MVarRef    = std::shared_ptr<syncing::MVar<U>>;
  using StateValue = std::variant<Try<T>, callback<T>>;

  MVarRef<StateValue>              storage_;
  syncing::MVar<void>              has_value_;
  syncing::MVar<executor_base_ref> executor_;

public:
  shared_state()
    : storage_(std::make_shared<syncing::MVar<StateValue>>()) {
  }

  /// Call only from producer thread
  void set_result(Try<T>&& result) {
    if (has_result()) {
      // If we already have the value, what to do? ¯\_(ツ)_/¯
    } else if (has_callback()) {
      callback<T> handler = std::get<callback<T>>(storage_->Take());
      storage_->Put(std::move(result));
      has_value_.Put();
      do_callback(std::move(handler));
    } else {
      storage_->Put(std::move(result));
      has_value_.Put();
    }
  }

  [[nodiscard]] bool has_result() const {
    bool const not_empty = !storage_->IsEmpty();
    bool const has_value = not_empty && storage_->ReadOnly().index() == 0;
    return has_value;
  }

  [[nodiscard]] bool has_callback() const {
    bool const not_empty    = !storage_->IsEmpty();
    bool const has_callback = not_empty && storage_->ReadOnly().index() == 1;
    return has_callback;
  }

  Try<T> get_result() {
    // We are waiting for the user value to be saved in the storage_
    has_value_.ReadOnly();

    // Then we try to get this value from the storage_
    return std::get<Try<T>>(storage_->ReadOnly());
  }

  /// Call only from consumer thread
  void set_executor(executor_base_ref&& exec) {
    assert(executor_.IsEmpty());
    executor_.Put(std::move(exec));
  }

  /// May call from any thread
  executor_base_ref get_executor() {
    return executor_.IsEmpty() ? nullptr : executor_.ReadOnly();
  }

  void set_callback(callback<T>&& callback) {
    if (has_callback()) {
      // If we already have the callback, what to do? ¯\_(ツ)_/¯
    } else if (has_result()) {
      do_callback(std::move(callback));
    } else {
      storage_->Put(std::move(callback));
    }
  }

private:
  void do_callback(callback<T>&& callback) {
    if (executor_base_ref executor = get_executor(); executor != nullptr) {
      executor->submit([callback = std::move(callback), storage = storage_]() mutable {
        callback(std::get<Try<T>>(storage->ReadOnly()));
      });
    } else {
      callback(std::get<Try<T>>(storage_->ReadOnly()));
    }
  }
};

template <typename T>
using state_ref = std::shared_ptr<shared_state<T>>;

template <typename T>
inline state_ref<T> make_shared_state() {
  return std::make_shared<shared_state<T>>();
}

template <typename T>
class hold_state {
public:
  hold_state(hold_state const& that)            = delete;
  hold_state& operator=(hold_state const& that) = delete;

protected:
  explicit hold_state(state_ref<T> state)
    : state_(std::move(state)) {
  }

  hold_state(hold_state&& that) noexcept            = default;
  hold_state& operator=(hold_state&& that) noexcept = default;
  ~hold_state()                                     = default;

  state_ref<T> release_state() {
    check_state();
    return std::move(state_);
  }

  state_ref<T> const& get_state() const {
    check_state();
    return state_;
  }

  bool has_state() const {
    return static_cast<bool>(state_);
  }

  void check_state() const {
    assert(has_state());
  }

protected:
  state_ref<T> state_;
};
}  // namespace dsac