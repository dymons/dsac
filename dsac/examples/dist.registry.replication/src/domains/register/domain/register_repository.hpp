#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/memory/shared_ptr.hpp>

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

  virtual auto write(register_value_object const& dto) -> void = 0;
  virtual auto reset() -> void                        = 0;
  virtual auto read() -> std::optional<register_value_object>  = 0;
};

using register_repository_ref = shared_ptr<register_repository>;

}  // namespace dsac::domain
