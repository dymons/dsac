#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

#include <cstdint>

namespace dsac::application::command::replica {

struct write_register_command final {
  std::int32_t value{};
  std::size_t  timestamp{};
};

class write_register_command_handler final {
  const domain::register_repository_ref register_repository_;

public:
  explicit write_register_command_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  auto handle(write_register_command const& command) const -> void;
};

}  // namespace dsac::application::command::replica
