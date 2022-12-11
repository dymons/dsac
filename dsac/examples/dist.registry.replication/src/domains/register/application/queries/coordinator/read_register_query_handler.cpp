#include <examples/dist.registry.replication/src/domains/register/application/queries/coordinator/read_register_query_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

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
  dynamic_array<std::optional<register_dto>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor_);
    responses.push_back(replica->read());
  }

  bool const has_register_state_dto =
      std::any_of(responses.begin(), responses.end(), [](std::optional<register_dto> const& response) {
        return response.has_value();
      });
  if (!has_register_state_dto) {
    return std::nullopt;
  }

  std::optional<register_dto> max_register_dto;
  for (std::optional<register_dto> const& r : responses) {
    if (!r.has_value()) {
      continue;
    }

    bool const is_register_undefined = !max_register_dto.has_value();
    bool const is_register_olden     = !is_register_undefined && r->get_timestamp() > max_register_dto->get_timestamp();
    if (is_register_undefined || is_register_olden) {
      max_register_dto = r;
    }
  }

  if (!max_register_dto.has_value()) {
    return std::nullopt;
  }

  return register_state_dto::hydrate(max_register_dto->get_value(), max_register_dto->get_timestamp());
}

}  // namespace dsac::application::query::coordinator