#include <examples/dist.registry.replication//models/json.hpp>
#include <examples/dist.registry.replication//models/request.hpp>
#include <examples/dist.registry.replication//models/response.hpp>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace dsac {

template <typename Message>
auto from_json(const nlohmann::json& j, Message& r) -> void {
  if constexpr (requires { r.value; }) {
    if (j.contains("value")) {
      if (!j["value"].is_string()) {
        throw parse_exception{fmt::format("The value is incorrect, expected string")};
      }
      r.value = j["value"].get<std::string>();
    }
  }
  if constexpr (requires { r.timestamp; }) {
    if (j.contains("timestamp")) {
      if (!j["timestamp"].is_string()) {
        throw parse_exception{fmt::format("The timestamp is incorrect, expected string")};
      }

      std::string timestamp = j["timestamp"].get<std::string>();

      std::tm t       = {};
      double  seconds = .0f;
      if (sscanf(  // NOLINT(cert-err34-c)
              timestamp.c_str(),
              "%04d-%02d-%02dT%02d:%02d:%lfZ",
              &t.tm_year,
              &t.tm_mon,
              &t.tm_mday,
              &t.tm_hour,
              &t.tm_min,
              &seconds) != 6) {
        throw parse_exception{fmt::format("The timestamp is incorrect, expected ISO8601 format timestamp")};
      }
      t.tm_sec = static_cast<int>(seconds);
      t.tm_mon -= 1;
      t.tm_year -= 1900;
      t.tm_isdst = -1;

      r.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&t));
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
      json["timestamp"] = fmt::format("{:%Y-%m-%dT%H:%M:%S%z}", r.timestamp.value());
    }
  }
  return json;
}

template auto from_json<request>(const nlohmann::json& j, request& r) -> void;
template auto to_json<request>(request const& r) -> nlohmann::json;

template auto from_json<response>(const nlohmann::json& j, response& r) -> void;
template auto to_json<response>(response const& r) -> nlohmann::json;

}  // namespace dsac