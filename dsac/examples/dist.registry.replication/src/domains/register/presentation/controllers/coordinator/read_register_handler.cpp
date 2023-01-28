#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/read_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using application::command::coordinator::read_and_write_register_command_handler;
using dsac::presentation::web::register_replica_client;
using infrastructure::quorum::majority_quorum_policy;

namespace {

auto make_response(domain::register_value_object const& register_state_dto) -> nlohmann::json {
  nlohmann::json response_json;
  response_json["value"]     = register_state_dto.get_value().get();
  response_json["timestamp"] = register_state_dto.get_timestamp().get();
  return response_json;
}

auto get_default_quorum_policy() -> dsac::domain::policy::quorum_policy_ref {
  return make_shared<majority_quorum_policy>();
}

}  // namespace

auto read_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  dynamic_array<domain::replica_ref> replicas;
  std::ranges::transform(
      register_replica_client::factory::get_registered_keys(),
      std::back_inserter(replicas),
      [this](std::string const& replica_name) -> domain::replica_ref {
        return shared_ptr<register_replica_client>(
            register_replica_client::factory::construct(replica_name, get_executor()).release()
        );
      }
  );

  read_and_write_register_command_handler command_handler{std::move(replicas), get_default_quorum_policy()};
  if (auto const register_value = command_handler.handle(); register_value.has_value()) {
    return make_response(register_value.value());
  }

  throw not_found{"The register is not initialized"};
}

}  // namespace dsac::presentation::coordinator
