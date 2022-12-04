#include <examples/dist.registry.replication/configuration/peers.hpp>
#include <examples/dist.registry.replication/consensus/factory.hpp>
#include <examples/dist.registry.replication/models/json.hpp>
#include <examples/dist.registry.replication/transport/httplib.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/memory/shared_ptr.hpp>

#include <nlohmann/json.hpp>

namespace {

auto make_server() -> dsac::shared_ptr<httplib::Server> {
  dsac::shared_ptr<httplib::Server> server = dsac::make_shared<httplib::Server>();
  std::set<std::string> const       topics = dsac::abd::factory::get_registered_keys();
  for (std::string const& topic : topics) {
    server->Post(topic, [topic, topics](const httplib::Request& request, httplib::Response& response) {
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
              response.status = 404;
              response.set_content(to_string(nlohmann::json{{"error_message", error}}), "text/json");
            });
      } catch (dsac::parse_exception const& exception) {
        response.status = 500;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      }
    });
  }

  server->Get(
      "/ping", []([[maybe_unused]] const httplib::Request& request, [[maybe_unused]] httplib::Response& response) {

      });

  return server;
}

}  // namespace

int main() {
  dsac::dynamic_array<dsac::peer> const peers    = dsac::get_static_configuration();
  dsac::executor_base_ref               executor = dsac::make_static_thread_pool(peers.size());

  for (dsac::peer const& peer : peers) {
    executor->submit([server = make_server(), peer] { server->listen(peer.get_host(), peer.get_port()); });
  }

  executor->join();
}
