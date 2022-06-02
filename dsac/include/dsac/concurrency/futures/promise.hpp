#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/state.hpp>

namespace dsac {

class promise_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class future_already_exposed : public promise_exception {
public:
  future_already_exposed()
    : promise_exception("Future is already exposed from promise") {
  }
};

template <typename T>
class promise final : public hold_state<T> {
  /*!
    \brief
        Expose a protected member release_state and state_ of base as private member of derived.
  */
  using hold_state<T>::state_;
  using hold_state<T>::release_state;

public:
  /*!
    \brief
        Expose a protected constructor of base as public member of derived.
  */
  using hold_state<T>::hold_state;

  // Modifiers

  /*!
    \brief
        Extract an dsac::future to get a future result

    \return
        Return the dsac::future

    \throw dsac::future_already_exposed
        The future was already exposed

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        dsac::future<int> future = promise.make_future();
        ...
    \endcode
  */
  [[nodiscard]] future<T> make_future();

  /*!
    \brief
        Initialize an dsac::result with different type of user-values

  \return
        Return the dsac::future

    \throw dsac::future_already_exposed
        The future was already exposed

    \ingroup
        DsacConcurrency

    \code
        dsac::promise<int> promise;
        promise.set(10);
        promise.set(dsac::result<int>(10));
        promise.set(std::current_exception{});
        ...
    \endcode
  */
  void set(T value);
  void set(result<T>&& result);
  void set(std::exception_ptr&& exception);

private:
  /// Determine that the dsac::future has already been exposed from dsac::promise
  bool future_extracted_{false};
};

}  // namespace dsac

#define CONCURRENCY_PROMISE_HPP
#include <dsac/concurrency/futures/detail/promise-inl.hpp>
#undef CONCURRENCY_PROMISE_HPP
