#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::replica {

using application::command::replica::write_register_command;
using application::command::replica::write_register_command_handler;
using infrastructure::inmemory::register_repository;

namespace {

auto from_json(const nlohmann::json& request) -> replica::write_register_command {
  if (!request.contains("value") || !request["value"].is_number_integer()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!request.contains("timestamp") || !request["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  return replica::write_register_command{
      .new_register_value =
          domain::register_value_object{
              domain::register_value{request["value"].get<std::int32_t>()},
              domain::register_timestamp{request["timestamp"].get<std::size_t>()},
          },
  };
}

}  // namespace

auto write_register_handler::handle(nlohmann::json const& request) -> nlohmann::json {
  write_register_command command = from_json(request);

  write_register_command_handler write_register_command_handler{assume_not_null(make_shared<register_repository>())};
  write_register_command_handler.handle(command);

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::replica
