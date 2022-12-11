#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

namespace dsac::application::command::coordinator {

using domain::register_dto;
using presentation::web::register_replica_client;

auto write_register_command_handler::handle(write_register_command const& command) -> void {
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica_client = register_replica_client::factory::construct(key);
    replica_client->write(register_dto::hydrate(command.value, command.timestamp));
  }
}

}  // namespace dsac::application::command::coordinator