#include <examples/dist.registry.replication/src/domains/register/application/queries/replica/read_register_query_handler.hpp>

namespace dsac::application::query::replica {

auto read_register_query_handler::handle() & -> std::optional<domain::register_value_object> {
  return register_repository_->read();
}

}  // namespace dsac::application::query::replica