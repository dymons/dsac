#include <examples/dist.registry.replication/src/domains/register/application/queries/coordinator/read_register_query_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/read_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

using application::query::coordinator::read_register_query;
using application::query::coordinator::read_register_query_handler;
using application::query::coordinator::register_state_dto;

namespace {

auto make_response(register_state_dto const& register_state_dto) -> nlohmann::json {
  nlohmann::json response_json;
  response_json["value"]     = register_state_dto.get_value();
  response_json["timestamp"] = register_state_dto.get_timestamp();
  return response_json;
}

}  // namespace

auto read_register_handler::handle([[maybe_unused]] nlohmann::json const& request) -> nlohmann::json {
  read_register_query_handler read_register_query_handler{get_executor()};
  std::optional const         register_state_dto = read_register_query_handler.handle(read_register_query{});
  if (!register_state_dto.has_value()) [[unlikely]] {
    throw not_found{"The register is not initialized"};
  }

  return make_response(register_state_dto.value());
}

}  // namespace dsac::presentation::coordinator
