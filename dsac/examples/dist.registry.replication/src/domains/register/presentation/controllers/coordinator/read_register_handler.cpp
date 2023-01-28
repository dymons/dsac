#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/read_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>
#include "examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp"

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using application::command::coordinator::read_and_write_register_command_handler;
using infrastructure::quorum::majority_quorum_policy;

namespace {

auto make_response(domain::register_value_object const& register_state_dto) -> nlohmann::json {
  nlohmann::json response_json;
  response_json["value"]     = register_state_dto.get_value();
  response_json["timestamp"] = register_state_dto.get_timestamp();
  return response_json;
}

}  // namespace

auto read_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  read_and_write_register_command_handler read_register_query_handler{
      get_executor(), make_shared<majority_quorum_policy>()};
  std::optional const register_state_dto = read_register_query_handler.handle();
  if (!register_state_dto.has_value()) [[unlikely]] {
    throw not_found{"The register is not initialized"};
  }

  return make_response(register_state_dto.value());
}

}  // namespace dsac::presentation::coordinator
