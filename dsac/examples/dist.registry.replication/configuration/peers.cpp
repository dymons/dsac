#include <examples/dist.registry.replication/configuration/peers.hpp>
#include <examples/dist.registry.replication/models/json.hpp>
#include <examples/dist.registry.replication/transport/httplib.hpp>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace dsac {

auto peer::execute(std::string const& topic, request const& request) const -> dsac::expected<response, std::string> {
  httplib::Result const response = httplib::Client{host_, port_}.Post(topic, to_string(to_json(request)), "text/json");
  if (response) {
    return nlohmann::json::parse(response.value().body).get<dsac::response>();
  }
  return make_unexpected(fmt::format("The request to {} is failed with code {}", topic, to_string(response.error())));
}

auto peer::get_host() const -> std::string {
  return host_;
}

auto peer::get_port() const -> int {
  return port_;
}

auto get_static_configuration() -> dynamic_array<peer> {
  return {{"0.0.0.0", 8080}, {"0.0.0.0", 8081}, {"0.0.0.0", 8082}};
}

}  // namespace dsac