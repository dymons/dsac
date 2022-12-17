#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::application::command::replica {

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

}  // namespace dsac::application::command::replica

namespace dsac::presentation::replica {

using application::command::replica::write_register_command;
using application::command::replica::write_register_command_handler;
using infrastructure::inmemory::register_repository;

auto write_register_handler::handle(nlohmann::json const& request) -> nlohmann::json {
  auto const write_register_command = request.get<replica::write_register_command>();

  write_register_command_handler write_register_command_handler{make_shared<register_repository>()};
  write_register_command_handler.handle(write_register_command);

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::replica
