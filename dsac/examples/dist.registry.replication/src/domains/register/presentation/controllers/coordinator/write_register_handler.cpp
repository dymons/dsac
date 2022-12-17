#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/detail/write_request.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/write_register_handler.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using command         = application::command::coordinator::write_register_command;
using command_handler = application::command::coordinator::write_register_command_handler;

auto write_register_handler::handle(nlohmann::json const& request_json) -> nlohmann::json {
  auto const request = request_json.get<write_request_dto>();

  command_handler{get_executor(), request.quorum_policy}.handle(command::hydrate(request.value, request.timestamp));

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::coordinator
