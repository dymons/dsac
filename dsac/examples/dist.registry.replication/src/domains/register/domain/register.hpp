#pragma once

#include <cstdint>

namespace dsac::domain {

class register_dto final {
  std::int32_t value_{};
  std::size_t  timestamp_{};

  register_dto(std::int32_t value, std::size_t timestamp);

public:
  register_dto(const register_dto&)                = default;
  register_dto(register_dto&&) noexcept            = default;
  register_dto& operator=(const register_dto&)     = default;
  register_dto& operator=(register_dto&&) noexcept = default;
  ~register_dto()                                  = default;

  [[nodiscard]] static register_dto hydrate(std::int32_t value, std::size_t timestamp) noexcept;
  [[nodiscard]] auto                get_value() const noexcept -> std::int32_t;
  [[nodiscard]] auto                get_timestamp() const noexcept -> std::size_t;
};

bool operator>(register_dto const& lhs, register_dto const& rha);

}  // namespace dsac::domain
