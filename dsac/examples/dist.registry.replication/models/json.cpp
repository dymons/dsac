#include <examples/dist.registry.replication/models/json.hpp>
#include <examples/dist.registry.replication/models/request.hpp>
#include <examples/dist.registry.replication/models/response.hpp>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace dsac {

template <typename Message>
auto from_json(const nlohmann::json& j, Message& r) -> void {
  if constexpr (requires { r.value; }) {
    if (j.contains("value")) {
      if (!j["value"].is_number_integer()) {
        throw parse_exception{fmt::format("The value is incorrect, expected is_number_integer")};
      }
      r.value = j["value"].get<int>();
    }
  }
  if constexpr (requires { r.timestamp; }) {
    if (j.contains("timestamp")) {
      if (!j["timestamp"].is_number_integer()) {
        throw parse_exception{fmt::format("The timestamp is incorrect, expected string")};
      }

      r.timestamp = j["timestamp"].get<std::size_t>();
    }
  }
}

template <typename Message>
auto to_json(Message const& r) -> nlohmann::json {
  nlohmann::json json;
  if constexpr (requires { r.value; }) {
    if (r.value.has_value()) {
      json["value"] = r.value.value();
    }
  }
  if constexpr (requires { r.timestamp; }) {
    if (r.timestamp.has_value()) {
      json["timestamp"] = r.timestamp.value();
    }
  }
  return json;
}

template auto from_json<request>(const nlohmann::json& j, request& r) -> void;
template auto to_json<request>(request const& r) -> nlohmann::json;

template auto from_json<response>(const nlohmann::json& j, response& r) -> void;
template auto to_json<response>(response const& r) -> nlohmann::json;

}  // namespace dsac