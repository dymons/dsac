#pragma once

#include <dsac/functional/strong_type.hpp>

#include <cstdint>

namespace dsac::domain {

using register_value     = strong_type<std::int32_t, struct RegisterValue>;
using register_timestamp = strong_type<std::size_t, struct RegisterMonotonicTimestamp>;

class register_value_object final {
  register_value     value_;
  register_timestamp timestamp_;

public:
  // Constructors

  register_value_object(register_value value, register_timestamp timestamp)
    : value_(value)
    , timestamp_(timestamp) {
  }

  // Observers

  [[nodiscard]] auto get_value() const noexcept -> std::int32_t;
  [[nodiscard]] auto get_timestamp() const noexcept -> std::size_t;
};

bool operator>(register_value_object const& lhs, register_value_object const& rhs);
bool operator==(register_value_object const& lhs, register_value_object const& rhs);

}  // namespace dsac::domain
