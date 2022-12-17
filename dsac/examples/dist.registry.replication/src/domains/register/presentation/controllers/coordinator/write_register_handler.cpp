#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::application::command::coordinator {

void from_json(const nlohmann::json& request, write_register_command& p) {
  if (!request.contains("value") || !request["value"].is_number_integer()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!request.contains("timestamp") || !request["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  p.value     = request["value"].get<std::int32_t>();
  p.timestamp = request["timestamp"].get<std::size_t>();
}

}  // namespace dsac::application::command::coordinator

namespace dsac::presentation::coordinator {

using application::command::coordinator::write_register_command;
using application::command::coordinator::write_register_command_handler;
using infrastructure::quorum::majority_quorum_policy;

auto write_register_handler::handle(nlohmann::json const& request) -> nlohmann::json {
  auto const write_register_command = request.get<coordinator::write_register_command>();

  write_register_command_handler write_register_command_handler{get_executor(), make_shared<majority_quorum_policy>()};
  write_register_command_handler.handle(write_register_command);

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::coordinator
