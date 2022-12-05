#include <examples/dist.registry.replication/configuration/peers.hpp>
#include <examples/dist.registry.replication/models/json.hpp>
#include <examples/dist.registry.replication/transport/httplib.hpp>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace dsac {

auto peer::execute(std::string topic, request request) -> expected<response, std::string> {
  httplib::Result const response =
      httplib::Client{get_host(), get_port()}.Post(topic, to_string(to_json(request)), "text/json");
  if (response) {
    return nlohmann::json::parse(response.value().body).get<dsac::response>();
  }
  return make_unexpected(fmt::format("The request to {} is failed with code {}", topic, to_string(response.error())));
}

}  // namespace dsac