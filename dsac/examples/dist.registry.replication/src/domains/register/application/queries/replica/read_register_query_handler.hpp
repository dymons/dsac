#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

#include <cstdint>
#include <optional>

namespace dsac::application::query::replica {

class read_register_query_handler final {
  domain::register_repository_ref register_repository_;

public:
  explicit read_register_query_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  [[nodiscard]] auto handle() & -> std::optional<domain::register_value_object>;
};

}  // namespace dsac::application::query::replica
