#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/callback.hpp>
#include <dsac/concurrency/futures/state.hpp>

namespace dsac {

class future_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class future_no_executor : public future_exception {
public:
  future_no_executor()
    : future_exception("No executor provided to via") {
  }
};

class future_no_callback : public future_exception {
public:
  future_no_callback()
    : future_exception("No callback provided to subscribe") {
  }
};

template <typename T>
class promise;

template <typename T>
class future final : public hold_state<T> {
  /*!
    \brief
        Allow dsac::promise to create an object dsac::future through a private constructor.
  */
  friend class promise<T>;

  /*!
    \brief
        Create an object dsac::future with error through a private constructor.
  */
  template <typename U>
  friend future<U> make_future_on_error(const char* message);

  /*!
    \brief
        Expose a protected member release_state of base as private member of derived.
  */
  using hold_state<T>::release_state;

public:
  // Observers

  /*!
    \brief
        Get the result obtained through the object dsac::promise.

    \return
        Return the result obtained from dsac::promise

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        dsac::future<int> future = promise.make_future();
        ...
        promise.set_value(10);
        dsac::result<int> value = future.get();
        do_stuff(value.value_or_throw());
    \endcode
  */
  result<T> get() &&;

  // Modifiers

  /*!
    \brief
        Set the execution environment within which the user-specified callback will be executed.

    \return
        Return the dsac::future with user-specified execution environment

    \throw dsac::future_no_executor
        The executor is nullptr and does not have a value

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        dsac::future<int> future = promise.make_future().via(executor);
        ...
    \endcode
  */
  future<T> via(executor_base_ref exec) &&;

  /*!
    \brief
        Set the callback to be executed when the value is received via dsac::promise.

    \return
        Return the dsac::future with user-specified callback

    \throw dsac::future_no_callback
        The callback is nullptr does not have a value

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        dsac::future<int> future = promise.make_future().subscribe([](auto result){
           do_stuff(result.value_or_throw());
        });
        ...
    \endcode
  */
  future<T> subscribe(callback<T> callback) &&;

  /*!
    \brief
        Creating a chain of deferred calls (Fluent interface).

    \return
        Return the dsac::future with user-specified callback

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        dsac::future<int> future = promise.make_future().then([](auto result) {
          return do_stuff(result.value_or_throw());
        }).then([](auto result) {
          return do_other_stuff(result.value_or_throw());
        });
        ...
    \endcode
  */
  template <typename F>
  auto then(F&& continuation) &&;

private:
  // Constructors

  /*!
    \brief
        User constructor, constructs a future with predefined shared state.
  */
  explicit future(state_ref<T> state);
};

template <typename T>
[[nodiscard]] future<T> make_future_on_error(const char* message);

}  // namespace dsac

#define CONCURRENCY_FUTURE_HPP
#include <dsac/concurrency/futures/detail/future-inl.hpp>
#undef CONCURRENCY_FUTURE_HPP
