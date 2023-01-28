#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/reset_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/reset_register_handler.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::replica {

using application::command::replica::reset_register_command_handler;
using infrastructure::inmemory::register_repository;

auto reset_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  reset_register_command_handler reset_register_command_handler{make_shared<register_repository>()};
  reset_register_command_handler.handle();
  return {};
}

}  // namespace dsac::presentation::replica
