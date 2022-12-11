#include <examples/dist.registry.replication/src/domains/register/application/queries/coordinator/read_register_query_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::application::query::coordinator {

using domain::register_dto;
using presentation::web::register_replica_client;

register_state_dto::register_state_dto(std::int32_t value, std::size_t timestamp)
  : value_(value)
  , timestamp_(timestamp) {
}

register_state_dto register_state_dto::hydrate(std::int32_t value, std::size_t timestamp) noexcept {
  return {value, timestamp};
}

auto register_state_dto::get_value() const noexcept -> std::int32_t {
  return value_;
}

auto register_state_dto::get_timestamp() const noexcept -> std::size_t {
  return timestamp_;
}

auto read_register_query_handler::handle([[maybe_unused]] read_register_query const& query) const
    -> std::optional<register_state_dto> {
  dynamic_array<future<register_dto>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor_);
    responses.push_back(replica->read());
  }

  std::size_t const                           quorum           = quorum_policy_->quorum(responses.size());
  future<dynamic_array<result<register_dto>>> quorum_future    = first_n(std::move(responses), quorum);
  dynamic_array<result<register_dto>>         quorum_responses = std::move(quorum_future).get().value_or_throw();

  std::optional<register_dto> max_register_state;
  for (result<register_dto> const& r : quorum_responses) {
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

  if (!max_register_state.has_value()) {
    return std::nullopt;
  }

  return register_state_dto::hydrate(max_register_state->get_value(), max_register_state->get_timestamp());
}

}  // namespace dsac::application::query::coordinator