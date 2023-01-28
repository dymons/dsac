#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

namespace dsac::infrastructure::inmemory {

class register_repository final : public domain::register_repository {
public:
  register_repository()                                          = default;
  register_repository(const register_repository&)                = default;
  register_repository(register_repository&&) noexcept            = default;
  register_repository& operator=(const register_repository&)     = default;
  register_repository& operator=(register_repository&&) noexcept = default;
  ~register_repository() override                                = default;

  auto write(domain::register_value_object const& dto) -> void override;
  auto reset() -> void override;
  auto read() -> std::optional<domain::register_value_object> override;
};

}  // namespace dsac::infrastructure::inmemory