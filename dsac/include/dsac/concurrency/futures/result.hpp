#pragma once

#include <compare>
#include <exception>
#include <variant>

namespace dsac {

template <typename T>
class result final {
  std::variant<T, std::exception_ptr> store_;

public:
  // Constructors

  /*!
    \brief
        User constructor, constructs an result with user-specified value T or std::exception_ptr.
  */
  explicit result(T value);
  explicit result(std::exception_ptr exception);

  // Observers

  /*!
    \brief
        Get the value if not then throw.

    \throw std::rethrow_exception
        The result does not have a value
  */
  auto value_or_throw() & -> T&;
  auto value_or_throw() const& -> const T&;
  auto value_or_throw() && -> T&&;
  auto value_or_throw() const&& -> const T&&;

  /*!
    \brief
        Check that the result store a value.
  */
  [[nodiscard]] auto has_value() const noexcept -> bool;

  /*!
    \brief
        Check that the result store an exception.
  */
  [[nodiscard]] auto has_exception() const noexcept -> bool;

  /*!
    \brief
        Compare two results.
  */
  auto operator<=>(result<T> const& other) const -> bool = default;
};

}  // namespace dsac

#define CONCURRENCY_RESULT_HPP
#include <dsac/concurrency/futures/detail/result-inl.hpp>
#undef CONCURRENCY_RESULT_HPP
