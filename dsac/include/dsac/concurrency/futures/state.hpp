#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/callback.hpp>
#include <dsac/concurrency/futures/result.hpp>
#include <dsac/concurrency/synchronization/mvar.hpp>

#include <variant>

namespace dsac {

class shared_state_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class shared_state_already_satisfied : public shared_state_exception {
public:
  shared_state_already_satisfied()
    : shared_state_exception("Shared state already satisfied") {
  }
};

template <typename T>
class shared_state final {
  using state_value = std::variant<result<T>, callback<T>>;

  mvar_ref<state_value>   storage_;
  mvar<void>              has_value_;
  mvar<executor_base_ref> executor_;

public:
  shared_state()
    : storage_(std::make_shared<mvar<state_value>>()) {
  }

  /// Call only from producer thread
  void set_result(result<T>&& result) {
    if (has_result()) {
      throw shared_state_already_satisfied{};
    }

    if (has_callback()) {
      callback<T> handler = std::get<callback<T>>(storage_->take());
      storage_->put(std::move(result));
      has_value_.put();
      do_callback(std::move(handler));
    } else {
      storage_->put(std::move(result));
      has_value_.put();
    }
  }

  [[nodiscard]] bool has_result() const {
    const auto result = storage_->template try_with_lock([](state_value const& state) { return state.index() == 0; });
    return result.has_value() && result.value();
  }

  [[nodiscard]] bool has_callback() const {
    const auto result = storage_->template try_with_lock([](state_value const& state) { return state.index() == 1; });
    return result.has_value() && result.value();
  }

  result<T> get_result() {
    // We are waiting for the user value to be saved in the storage_
    has_value_.read_only();

    // Then we try to get this value from the storage_
    return std::get<result<T>>(storage_->read_only());
  }

  /// Call only from consumer thread
  void set_executor(executor_base_ref&& exec) {
    assert(executor_.is_empty());
    executor_.put(std::move(exec));
  }

  /// May call from any thread
  executor_base_ref get_executor() {
    return executor_.is_empty() ? nullptr : executor_.read_only();
  }

  void set_callback(callback<T>&& callback) {
    if (has_callback()) {
      // If we already have the callback, what to do? ¯\_(ツ)_/¯
    } else if (has_result()) {
      do_callback(std::move(callback));
    } else {
      storage_->put(std::move(callback));
    }
  }

private:
  void do_callback(callback<T>&& callback) {
    storage_->template try_with_lock([this, &callback](state_value const& state) -> bool {
      auto data = std::get<result<T>>(state);
      if (executor_base_ref executor = get_executor(); executor != nullptr) {
        executor->submit(
            [callback = std::move(callback), data = std::move(data)]() mutable { callback(std::move(data)); });
      } else {
        callback(std::move(data));
      }
      return true;
    });
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
