#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

namespace dsac::infrastructure::inmemory {

class register_repository final : public domain::register_repository_base {
public:
  auto write(domain::register_value_object const& dto) -> void final;
  auto reset() -> void final;
  auto read() -> std::optional<domain::register_value_object> final;
};

}  // namespace dsac::infrastructure::inmemory