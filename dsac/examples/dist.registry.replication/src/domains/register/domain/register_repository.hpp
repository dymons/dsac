#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/memory/shared_ptr.hpp>
#include <dsac/memory/not_null.hpp>

#include <optional>

namespace dsac::domain {

class register_repository_base {
public:
  register_repository_base()                                               = default;
  register_repository_base(const register_repository_base&)                = delete;
  register_repository_base(register_repository_base&&) noexcept            = delete;
  register_repository_base& operator=(const register_repository_base&)     = delete;
  register_repository_base& operator=(register_repository_base&&) noexcept = delete;
  virtual ~register_repository_base()                                      = default;

  virtual auto write(register_value_object const& object) -> void = 0;

  virtual auto reset() -> void = 0;

  virtual auto read() -> std::optional<register_value_object> = 0;
};

using register_repository_ref = not_null<shared_ptr<register_repository_base>>;

}  // namespace dsac::domain
