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
  [[nodiscard]] executor_base_ref get_executor() const;

  // Modifiers

  /*!
    \brief
        Initialize a shared state by dsac::result. Call only from producer thread.

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
  void set_result(result<T>&& result);

  /*!
    \brief
        Initialize an execution environment for user-specified callback. Call only from producer thread.

    \param exec
        An execution environment for user-specified callback

    \ingroup
        DsacConcurrency

    \code
          auto executor = dsac::make_static_thread_pool(4U);
          auto state = dsac::make_shared_state<int>{};
          state->set_executor(executor);
          ...
    \endcode
  */
  void set_executor(executor_base_ref exec);

  /*!
    \brief
        Initialize a shared state by dsac::callback. Call only from producer thread.

    \param result
            User-data for saving withing shared state

    \throw dsac::shared_state_already_satisfied
        Shared state does not support of initializing more than once by dsac::callback value

    \ingroup
        DsacConcurrency

    \code
          auto state = dsac::make_shared_state<int>{};
          state->set_callback({}(dsac::result<int> result) { do_stuff(result); });
    \endcode
  */
  void set_callback(callback<T>&& callback);

private:
  /*!
      \brief
          Perform dsac::result processing in a user-specified callback.
  */
  void do_callback(callback<T>&& callback);

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

  /*!
      \brief
          Value of shared state.
  */
  using state_value = std::variant<result<T>, callback<T>>;

  /// A place to store user data or user-specified callback
  mvar_ref<state_value> storage_;

  /// A synchronization primitive to provide blocking data waiting in storage_
  mvar<void> has_value_;

  /// User-specified environment for execution callback<T> from storage_
  executor_base_ref executor_;
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
  // Constructors

  explicit hold_state(state_ref<T> state = make_shared_state<T>());

  /*!
    \brief
        Copy constructor.
  */
  hold_state(hold_state const& that) = delete;

  /*!
    \brief
        Move constructor.
  */
  hold_state(hold_state&& that) noexcept = default;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~hold_state() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  hold_state& operator=(hold_state const& that) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  hold_state& operator=(hold_state&& that) noexcept = default;

protected:
  /*!
    \brief
        Release the current state and transfer ownership to another hold_state<T>.
  */
  state_ref<T> release_state() &&;

  /// State of the hold state
  state_ref<T> state_;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

}  // namespace dsac

#define CONCURRENCY_STATE_HPP
#include <dsac/concurrency/futures/detail/state-inl.hpp>
#undef CONCURRENCY_STATE_HPP
