#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/reset_register_command_handler.hpp>

namespace dsac::application::command::replica {

auto reset_register_command_handler::handle([[maybe_unused]] reset_register_command const& command) -> void {
  register_repository_->reset();
}

}  // namespace dsac::application::command::replica