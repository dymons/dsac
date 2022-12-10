#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/reset_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>

namespace dsac::application::command::replica {

using infrastructure::inmemory::register_repository;

auto reset_register_command_handler::handle([[maybe_unused]] reset_register_command const& command) -> void {
  register_repository{}.reset();
}

}  // namespace dsac::application::command::replica