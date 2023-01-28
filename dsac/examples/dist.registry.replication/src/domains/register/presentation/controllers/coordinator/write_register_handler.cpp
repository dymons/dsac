#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/detail/write_request.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/write_register_handler.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using application::command::coordinator::write_register_command;
using application::command::coordinator::write_register_command_handler;
using domain::policy::quorum_policy;
using dsac::infrastructure::quorum::majority_quorum_policy;

auto write_register_handler::handle(nlohmann::json const& request_json) -> nlohmann::json {
  auto request = request_json.get<write_request_dto>();
  if (nullptr == request.quorum_policy) {
    request.quorum_policy =
        shared_ptr{quorum_policy::factory::construct(majority_quorum_policy::get_type_name()).release()};
  }

  write_register_command_handler{get_executor(), request.quorum_policy}.handle(write_register_command{
      .object = domain::register_value_object{
          domain::register_value{request.value}, domain::register_timestamp{request.timestamp}}});

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::coordinator
