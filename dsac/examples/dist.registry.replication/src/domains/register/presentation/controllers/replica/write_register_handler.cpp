#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::replica {

using application::command::replica::write_register_command;
using application::command::replica::write_register_command_handler;

auto write_register_handler::handle(nlohmann::json const& request) -> nlohmann::json {
  if (!request.contains("value") || !request["value"].is_number_integer()) [[unlikely]] {
    throw invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!request.contains("timestamp") || !request["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  write_register_command write_register_command{};
  write_register_command.value     = request["value"].get<std::int32_t>();
  write_register_command.timestamp = request["timestamp"].get<std::size_t>();

  write_register_command_handler::handle(write_register_command);

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::replica
