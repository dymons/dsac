#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/policy/majority_quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/detail/write_request.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using application::command::coordinator::write_register_command;
using application::command::coordinator::write_register_command_handler;
using domain::policy::quorum_policy_base;
using dsac::infrastructure::quorum::majority_quorum_policy;
using dsac::presentation::web::register_replica_client;

namespace {

auto get_default_quorum_policy() -> dsac::domain::policy::quorum_policy_ref {
  return make_shared<majority_quorum_policy>();
}

}  // namespace

auto write_register_handler::handle(nlohmann::json const& request_json) -> nlohmann::json {
  auto request = request_json.get<write_request_dto>();
  if (nullptr == request.quorum_policy) {
    request.quorum_policy = get_default_quorum_policy();
  }

  dynamic_array<not_null<domain::replica_ref>> replicas;
  std::ranges::transform(
      register_replica_client::factory::get_registered_keys(),
      std::back_inserter(replicas),
      [this](std::string const& replica_name) -> not_null<domain::replica_ref> {
        return assume_not_null(shared_ptr<register_replica_client>(
            register_replica_client::factory::construct(replica_name, get_executor()).release()
        ));
      }
  );

  write_register_command_handler command_handler{std::move(replicas), request.quorum_policy};
  command_handler.handle(write_register_command{
      .new_register_value = domain::register_value_object{
          domain::register_value{request.value},         //
          domain::register_timestamp{request.timestamp}  //
      }});

  // We always confirm the client's record, even if we ignore it by timestamp.
  return {};
}

}  // namespace dsac::presentation::coordinator
