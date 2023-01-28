#pragma once

#include <dsac/functional/strong_type.hpp>

#include <cstdint>

namespace dsac::domain {

/*!
  \brief
      The register stores an integer type
*/
using register_value = strong_type<std::int32_t, struct RegisterValue>;

/*!
  \brief
      Monotonically increasing clocks are used for versioning
*/
using register_timestamp = strong_type<std::size_t, struct RegisterTimestamp>;

/*!
  \brief
      The entity of the subject area, represents a register value object
*/
class register_value_object final {
  register_value     value_;
  register_timestamp timestamp_;

public:
  // Constructors

  /*!
    \brief
        User constructor in a private scope. Used to create from the hydrate method.
  */
  register_value_object(register_value value, register_timestamp timestamp)
    : value_(value)
    , timestamp_(timestamp) {
  }

  // Observers

  [[nodiscard]] auto get_value() const noexcept -> register_value;
  [[nodiscard]] auto get_timestamp() const noexcept -> register_timestamp;
};

bool operator>(register_value_object const& lhs, register_value_object const& rhs);
bool operator==(register_value_object const& lhs, register_value_object const& rhs);

}  // namespace dsac::domain
