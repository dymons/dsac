#include <examples/dist.registry.replication/src/domains/register/presentation/controller_factory.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/detail/httplib.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <iostream>
#include <optional>
#include <thread>

#include <nlohmann/json.hpp>

namespace {

using dsac::presentation::controller;

template <typename T, typename U>
auto cast_to(U) -> T;

template <>
auto cast_to<int, char*>(char* string) -> int {  // NOLINT(readability-non-const-parameter)
  return std::stoi(string);
}

template <typename T, typename Iterator>
auto get_parameter_from_args_as(Iterator begin, Iterator end, std::string parameter) -> std::optional<T> try {
  Iterator it = std::find(begin, end, parameter);
  if (it != end && ++it != end) {
    return cast_to<T>(*it);
  }
  return std::nullopt;
} catch (std::exception const& exception) {
  std::cerr << "Unexpected exception caught with error message " << exception.what();
  return std::nullopt;
}

}  // namespace

int main(int args, char** argv) {
  std::optional<int> const port = get_parameter_from_args_as<int>(argv, std::next(argv, args), "--port");
  if (!port.has_value()) {
    std::cerr << "The server port is not set. Please specify the port using the key --port <port>";
    return -1;
  }

  dsac::executor_base_ref executor = dsac::make_static_thread_pool(std::thread::hardware_concurrency());

  httplib::Server             server;
  std::set<std::string> const topics = controller::factory::get_registered_keys();
  for (std::string const& topic : topics) {
    server.Post(topic, [topic, topics, executor](const httplib::Request& request, httplib::Response& response) {
      std::unique_ptr<controller> const topic_callback = controller::factory::construct(topic, executor);
      if (nullptr == topic_callback) {
        response.status = 404;
        response.set_content(
            fmt::format("The topic {} was not found. Please choose one of {}", topic, fmt::join(topics, ",")),
            "text/plain");
        return;
      }

      try {
        nlohmann::json const request_json =
            request.body.empty() ? nlohmann::json{} : nlohmann::json::parse(request.body);
        nlohmann::json const response_json = topic_callback->handle(request_json);
        response.status                    = 200;
        response.set_content(to_string(response_json), "text/json");
      } catch (dsac::presentation::invalid_argument const& exception) {
        response.status = 400;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      } catch (dsac::presentation::not_found const& exception) {
        response.status = 404;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      } catch (dsac::presentation::service_unavailable const& exception) {
        response.status = 503;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      } catch (dsac::presentation::exception const& exception) {
        response.status = 500;
        response.set_content(to_string(nlohmann::json{{"error_message", exception.what()}}), "text/json");
      }
    });
  }

  server.Get(
      "/ping", []([[maybe_unused]] const httplib::Request& request, [[maybe_unused]] httplib::Response& response) {
        (void)request;
      });

  server.listen("0.0.0.0", port.value());
  executor->join();
}