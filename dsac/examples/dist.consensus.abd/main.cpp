#include <examples/dist.consensus.abd/consensus/factory.hpp>
#include <examples/dist.consensus.abd/transport/httplib.hpp>

#include <nlohmann/json.hpp>

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
