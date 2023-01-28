#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>

namespace dsac::application::command::replica {

auto write_register_command_handler::handle(write_register_command const& command) & -> void {
  register_repository_->write(command.new_register_value);
}

}  // namespace dsac::application::command::replica