#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

#include <cstdint>
#include <optional>

namespace dsac::application::query::replica {

struct read_register_query final {};

class read_register_query_handler final {
  const domain::register_repository_ref register_repository_;

public:
  explicit read_register_query_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  [[nodiscard]] auto handle(read_register_query const& query) const -> std::optional<domain::register_value_object>;
};

}  // namespace dsac::application::query::replica
