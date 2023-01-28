#include <examples/dist.registry.replication/src/domains/register/application/queries/replica/read_register_query_handler.hpp>

namespace dsac::application::query::replica {

using domain::register_value_object;

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
  std::optional<domain::register_value_object> const value = register_repository_->read();
  if (value.has_value()) {
    return register_state_dto::hydrate(value->get_value(), value->get_timestamp());
  }
  return std::nullopt;
}

}  // namespace dsac::application::query::replica