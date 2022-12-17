#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace {

using dsac::domain::policy::quorum_policy;
using dsac::infrastructure::quorum::majority_quorum_policy;

struct write_request_dto final {
  std::int32_t                            value{};
  std::size_t                             timestamp{};
  dsac::domain::policy::quorum_policy_ref quorum_policy{nullptr};
};

void from_json(const nlohmann::json& request, write_request_dto& p) {
  if (!request.contains("value") || !request["value"].is_number_integer()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!request.contains("timestamp") || !request["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  std::string quorum_policy = majority_quorum_policy::get_type_name();
  if (request.contains("quorum_policy")) {
    if (!request["quorum_policy"].is_string()) [[unlikely]] {
      throw dsac::presentation::invalid_argument{
          "Input data is incorrect or the required field 'quorum_policy' is missing"};
    }
    quorum_policy = request["quorum_policy"].get<std::string>();
  }

  p.value         = request["value"].get<std::int32_t>();
  p.timestamp     = request["timestamp"].get<std::size_t>();
  p.quorum_policy = dsac::shared_ptr{quorum_policy::factory::construct(quorum_policy).release()};
}

}  // namespace

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
