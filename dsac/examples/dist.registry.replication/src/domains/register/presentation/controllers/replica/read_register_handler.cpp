#include <examples/dist.registry.replication/src/domains/register/application/queries/replica/read_register_query_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/read_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::replica {

using application::query::replica::read_register_query;
using application::query::replica::read_register_query_handler;
using infrastructure::inmemory::register_repository;

namespace {

auto make_response(domain::register_value_object const& object) -> nlohmann::json {
  nlohmann::json response_json;
  response_json["value"]     = object.get_value().get();
  response_json["timestamp"] = object.get_timestamp().get();
  return response_json;
}

}  // namespace

auto read_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  read_register_query_handler read_register_query_handler{make_shared<register_repository>()};

  std::optional const register_state_dto = read_register_query_handler.handle(read_register_query{});
  if (!register_state_dto.has_value()) [[unlikely]] {
    throw not_found{"The register is not initialized"};
  }

  return make_response(register_state_dto.value());
}

}  // namespace dsac::presentation::replica
