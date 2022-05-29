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

public:
  // Constructors

  /*!
      \brief
          Default constructor, constructs an empty shared_state.
  */
  shared_state();

  // Observers

  /*!
      \brief
          Checking what shared state initialized by dsac::result.
  */
  [[nodiscard]] bool has_result() const;

  /*!
      \brief
          Checking what shared state initialized by dsac::callback.
  */
  [[nodiscard]] bool has_callback() const;

  /*!
      \brief
          Extract dsac::result from shared state.
  */
  [[nodiscard]] result<T> get_result();

  /*!
      \brief
          Get the current execution environment for execution of user subscriptions. May call from any thread.
  */
  [[nodiscard]] executor_base_ref get_executor();

  // Modifiers

  /*!
    \brief
        Initialize a shared state by value. Call only from producer thread.

    \param result
        User-data for saving withing shared state

    \throw dsac::shared_state_already_satisfied
        Shared state does not support of initializing more than once by dsac::result value

    \ingroup
          DsacConcurrency

    \code
          auto state = dsac::make_shared_state<int>{};
          state->set_result(0);
    \endcode
  */
  void set_result(result<T>&& result) {
    throw_if_fulfilled_by_result();

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

  /// Call only from consumer thread
  void set_executor(executor_base_ref&& exec) {
    assert(executor_.is_empty());
    executor_.put(std::move(exec));
  }

  void set_callback(callback<T>&& callback) {
    throw_if_fulfilled_by_callback();

    if (has_result()) {
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

  /*!
      \brief
          Check if shared state already initialized by dsac::result then throw.
  */
  void throw_if_fulfilled_by_result();

  /*!
      \brief
          Check if shared state already initialized by dsac::callback then throw.
  */
  void throw_if_fulfilled_by_callback();

  mvar_ref<state_value>   storage_;
  mvar<void>              has_value_;
  mvar<executor_base_ref> executor_;
};

template <typename T>
using state_ref = dsac::shared_ptr<shared_state<T>>;

template <typename T>
inline state_ref<T> make_shared_state() {
  return dsac::make_shared<shared_state<T>>();
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

#define CONCURRENCY_STATE_HPP
#include <dsac/concurrency/futures/detail/state-inl.hpp>
#undef CONCURRENCY_STATE_HPP
