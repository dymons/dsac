#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

namespace dsac::application::command::replica {

struct reset_register_command final {};

class reset_register_command_handler {
  const domain::register_repository_ref register_repository_;

public:
  explicit reset_register_command_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  auto handle(reset_register_command const& command) -> void;
};

}  // namespace dsac::application::command::replica
