#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>

namespace dsac::application::command::replica {

using domain::register_dto;
using infrastructure::inmemory::register_repository;

auto write_register_command_handler::handle(write_register_command const& command) -> void {
  register_repository{}.write(register_dto::hydrate(command.value, command.timestamp));
}

}  // namespace dsac::application::command::replica