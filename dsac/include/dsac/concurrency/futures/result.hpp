#pragma once

#include <exception>
#include <variant>

namespace dsac {

template <typename T>
class result {
  std::variant<T, std::exception_ptr> store_;

public:
  // Constructors

  /*!
    \brief
        User constructor, constructs an result with user-specified value T or std::exception_ptr.
  */
  explicit result(T value);
  explicit result(std::exception_ptr&& exception);

  // Observers

  /*!
    \brief
        Get the value if not then throw.
  */
  const T& value_or_throw() const&;

  /*!
    \brief
        Check that the result store a value.
  */
  [[nodiscard]] bool has_value() const noexcept;

  /*!
    \brief
        Check that the result store an exception.
  */
  [[nodiscard]] bool has_exception() const noexcept;
};

/*!
  \brief
      Compare two results for equivalence.
*/
template <typename T>
bool operator==(result<T> const& p, result<T> const& b);
template <typename T>
bool operator<=>(result<T> const& p, result<T> const& b);

}  // namespace dsac

#define CONCURRENCY_RESULT_HPP
#include <dsac/concurrency/futures/detail/result-inl.hpp>
#undef CONCURRENCY_RESULT_HPP
