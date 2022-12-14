#include "read_and_write_register_command_handler.hpp"
#include "examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp"

#include "dsac/concurrency/futures/combine/first_n.hpp"
#include "dsac/container/dynamic_array.hpp"

namespace dsac::application::command::coordinator {

using domain::register_dto;
using presentation::web::register_replica_client;

namespace {

auto get_replica_states_from_quorum(
    executor_base_ref const& executor, domain::policy::quorum_policy_ref const& quorum_policy)
    -> dynamic_array<result<register_dto>> {
  dynamic_array<future<register_dto>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor);
    responses.push_back(replica->read());
  }

  std::size_t const                           quorum        = quorum_policy->quorum(responses.size());
  future<dynamic_array<result<register_dto>>> quorum_future = first_n(std::move(responses), quorum);

  return std::move(quorum_future).get().value_or_throw();
}

auto get_max_replica_state(dynamic_array<result<register_dto>> const& quorum) -> std::optional<register_dto> {
  std::optional<register_dto> max_register_state;
  for (result<register_dto> const& r : quorum) {
    if (!r.has_value()) {
      continue;
    }

    register_dto const register_state        = r.value_or_throw();
    bool const         is_register_undefined = !max_register_state.has_value();
    bool const         is_register_olden     = !is_register_undefined && register_state > max_register_state;
    if (is_register_undefined || is_register_olden) {
      max_register_state = register_state;
    }
  }
  return max_register_state;
}

auto make_write_phase_if_need(
    dynamic_array<result<register_dto>> const& replica_states,
    register_dto const&                        max_replica_state,
    executor_base_ref const&                   executor,
    domain::policy::quorum_policy_ref const&   quorum_policy) -> void {
  bool const are_replicas_consistent = std::all_of(
      replica_states.begin(), replica_states.end(), [&max_replica_state](result<register_dto> const& replica_state) {
        return replica_state.has_value() && replica_state.value_or_throw() == max_replica_state;
      });
  if (not are_replicas_consistent) {
    dynamic_array<future<void*>> responses;
    for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
      std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor);
      responses.push_back(
          replica->write(register_dto::hydrate(max_replica_state.get_value(), max_replica_state.get_timestamp())));
    }

    std::size_t const                    quorum           = quorum_policy->quorum(responses.size());
    future<dynamic_array<result<void*>>> quorum_future    = first_n(std::move(responses), quorum);
    dynamic_array<result<void*>>         quorum_responses = std::move(quorum_future).get().value_or_throw();
    for (result<void*> const& response : quorum_responses) {
      response.value_or_throw();
    }
  }
}

}  // namespace

auto read_and_write_register_command_handler::handle([[maybe_unused]] read_register_command const& query) const
    -> std::optional<register_dto> {
  auto const replica_states     = get_replica_states_from_quorum(executor_, quorum_policy_);
  auto const max_register_state = get_max_replica_state(replica_states);
  if (!max_register_state.has_value()) {
    return std::nullopt;
  }

  make_write_phase_if_need(replica_states, max_register_state.value(), executor_, quorum_policy_);

  return max_register_state;
}

}  // namespace dsac::application::command::coordinator