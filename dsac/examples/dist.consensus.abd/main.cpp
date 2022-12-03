#include <examples/dist.consensus.abd/consensus/factory.hpp>
#include <examples/dist.consensus.abd/transport/httplib.hpp>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace dsac {

class parse_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

void from_json(const nlohmann::json& j, request& r) {
  if (j.contains("key")) {
    if (!j["key"].is_string()) {
      throw parse_exception{fmt::format("The key is incorrect, expected string")};
    }
    r.key = j["key"].get<std::string>();
  }
  if (j.contains("value")) {
    if (!j["value"].is_string()) {
      throw parse_exception{fmt::format("The value is incorrect, expected string")};
    }
    r.value = j["value"].get<std::string>();
  }
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
    t.tm_isdst  = -1;

    r.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&t));
  }
}

auto to_json(const response& r) -> nlohmann::json {
  nlohmann::json json;
  if (r.value.has_value()) {
    json["value"] = r.value.value();
  }
  if (r.timestamp.has_value()) {
    json["timestamp"] = fmt::format("{:%Y-%m-%dT%H:%M:%S%z}", r.timestamp.value());
  }
  return json;
}

}  // namespace dsac

int main() {
  httplib::Server             server;
  std::set<std::string> const topics = dsac::abd::factory::get_registered_keys();
  for (std::string const& topic : topics) {
    server.Post(topic, [topic, topics](const httplib::Request& request, httplib::Response& response) {
      std::unique_ptr<dsac::abd> const topic_callback = dsac::abd::factory::construct(topic);
      if (nullptr == topic_callback) {
        response.status = 404;
        response.set_content(
            fmt::format("The topic {} was not found. Please choose one of {}", topic, fmt::join(topics, ",")),
            "text/plain");
        return;
      }

      try {
        topic_callback->execute(nlohmann::json::parse(request.body).get<dsac::request>())
            .map([&response](dsac::response const& r) {
              response.status = 200;
              response.set_content(to_string(to_json(r)), "text/json");
            })
            .map_error([&response](std::string const& error) {
              response.status = 400;
              response.set_content(to_string(nlohmann::json{{"error_message", error}}), "text/json");
            });
      } catch (dsac::parse_exception const& exception) {
        response.status = 500;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      }
    });
  }

  server.Get(
      "/ping", []([[maybe_unused]] const httplib::Request& request, [[maybe_unused]] httplib::Response& response) {

      });

  server.listen("0.0.0.0", 8080);
}
