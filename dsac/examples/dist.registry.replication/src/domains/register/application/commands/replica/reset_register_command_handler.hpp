#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

namespace dsac::application::command::replica {

class reset_register_command_handler final {
  domain::register_repository_ref register_repository_;

public:
  explicit reset_register_command_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  auto handle() & -> void;
};

}  // namespace dsac::application::command::replica
