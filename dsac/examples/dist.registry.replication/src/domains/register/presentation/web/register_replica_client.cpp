#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/read_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/replica/write_register_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/detail/httplib.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/async_via.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::web {

using presentation::replica::read_register_handler;
using presentation::replica::write_register_handler;

namespace {

const std::string kRequestEmpty = "";

auto to_json(domain::register_value_object const& request) -> std::string {
  nlohmann::json request_json;
  request_json["value"]     = request.get_value().get();
  request_json["timestamp"] = request.get_timestamp().get();
  return to_string(request_json);
}

auto from_json(nlohmann::json const& json) -> domain::register_value_object {
  if (!json.contains("value") || !json["value"].is_number_integer()) [[unlikely]] {
    throw invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!json.contains("timestamp") || !json["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  return {
      domain::register_value(json["value"].get<std::int32_t>()),
      domain::register_timestamp(json["timestamp"].get<std::size_t>())};
}

}  // namespace

auto register_replica_client::async_write(domain::register_value_object const& request) -> future<void*> {
  return async_via(get_executor(), [host = get_host(), port = get_port(), request = request]() -> void* {
    httplib::Client{host, port}.Post(write_register_handler::get_type_name(), to_json(request), "text/json");
    return nullptr;
  });
}

auto register_replica_client::async_read() -> future<domain::register_value_object> {
  return async_via(get_executor(), [host = get_host(), port = get_port()]() -> domain::register_value_object {
    httplib::Result const response =
        httplib::Client{host, port}.Post(read_register_handler::get_type_name(), kRequestEmpty, "text/json");
    if (!response) {
      throw service_unavailable{fmt::format("Failed to execute the request to host={}, port={}", host, port)};
    }

    nlohmann::json const response_json = nlohmann::json::parse(response.value().body);

    bool const has_error_message  = response_json.contains("error_message");
    bool const not_found_register = response->status == 404;
    if (has_error_message && not_found_register) {
      throw not_found{response_json["error_message"].get<std::string>()};
    }
    if (has_error_message) {
      throw service_unavailable{fmt::format("Unexpected error response from host={}, port={}", host, port)};
    }

    return from_json(response_json);
  });
}

}  // namespace dsac::presentation::web
