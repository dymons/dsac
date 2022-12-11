#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>

namespace dsac::application::command::replica {

using domain::register_dto;

auto write_register_command_handler::handle(write_register_command const& command) const -> void {
  register_repository_->write(register_dto::hydrate(command.value, command.timestamp));
}

}  // namespace dsac::application::command::replica