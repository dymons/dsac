#pragma once

#include <dsac/concurrency/executors/executor.hpp>

#include <cstdint>
#include <optional>

namespace dsac::application::query::coordinator {

class register_state_dto final {
  std::int32_t value_{};
  std::size_t  timestamp_{};

  register_state_dto(std::int32_t value, std::size_t timestamp);

public:
  register_state_dto(const register_state_dto&)                = default;
  register_state_dto(register_state_dto&&) noexcept            = default;
  register_state_dto& operator=(const register_state_dto&)     = default;
  register_state_dto& operator=(register_state_dto&&) noexcept = default;
  ~register_state_dto()                                        = default;

  [[nodiscard]] static register_state_dto hydrate(std::int32_t value, std::size_t timestamp) noexcept;
  [[nodiscard]] auto                      get_value() const noexcept -> std::int32_t;
  [[nodiscard]] auto                      get_timestamp() const noexcept -> std::size_t;
};

struct read_register_query final {};

class read_register_query_handler final {
  const executor_base_ref executor_;

public:
  explicit read_register_query_handler(executor_base_ref executor)
    : executor_(std::move(executor)) {
  }

  auto handle(read_register_query const& query) const -> std::optional<register_state_dto>;
};

}  // namespace dsac::application::query::coordinator
