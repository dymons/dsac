#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register_repository_base.hpp>

#include <cstdint>

namespace dsac::application::command::replica {

struct write_register_command final {
  domain::register_value_object new_register_value;
};

class write_register_command_handler final {
  domain::register_repository_ref register_repository_;

public:
  explicit write_register_command_handler(domain::register_repository_ref register_repository)
    : register_repository_(std::move(register_repository)) {
  }

  auto handle(write_register_command const& command) & -> void;
};

}  // namespace dsac::application::command::replica
