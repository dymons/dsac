#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>

namespace dsac::application::command::replica {

using domain::register_value_object;

auto write_register_command_handler::handle(write_register_command const& command) const -> void {
  register_repository_->write(
      register_value_object(domain::register_value(command.value), domain::register_timestamp(command.timestamp))
  );
}

}  // namespace dsac::application::command::replica