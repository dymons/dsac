#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/reset_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/reset_register_handler.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::replica {

using application::command::replica::reset_register_command;
using application::command::replica::reset_register_command_handler;

auto reset_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  reset_register_command_handler::handle(reset_register_command{});
  return {};
}

}  // namespace dsac::presentation::replica
