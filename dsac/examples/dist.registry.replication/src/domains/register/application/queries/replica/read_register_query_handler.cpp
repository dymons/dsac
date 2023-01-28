#include <examples/dist.registry.replication/src/domains/register/application/queries/replica/read_register_query_handler.hpp>

namespace dsac::application::query::replica {

auto read_register_query_handler::handle([[maybe_unused]] read_register_query const& query) const
    -> std::optional<domain::register_value_object> {
  std::optional<domain::register_value_object> const value = register_repository_->read();
  if (value.has_value()) {
    return value.value();
  }
  return std::nullopt;
}

}  // namespace dsac::application::query::replica