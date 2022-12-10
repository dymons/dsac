#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <optional>

namespace dsac::domain {

class register_repository {
public:
  register_repository()                                          = default;
  register_repository(const register_repository&)                = default;
  register_repository(register_repository&&) noexcept            = default;
  register_repository& operator=(const register_repository&)     = default;
  register_repository& operator=(register_repository&&) noexcept = default;
  virtual ~register_repository()                                 = default;

  virtual auto write(register_dto const& dto) -> void = 0;
  virtual auto reset() -> void                        = 0;
  virtual auto read() -> std::optional<register_dto>  = 0;
};

}  // namespace dsac::domain
