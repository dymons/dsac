#include <examples/dist.registry.replication/src/domains/register/presentation/controller_factory.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/detail/httplib.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <thread>

#include <nlohmann/json.hpp>

namespace {

using dsac::presentation::controller;

template <typename T, typename U>
[[nodiscard]] auto cast_to(U) -> T;

template <>
[[nodiscard]] auto cast_to<int, std::string_view>(std::string_view string) -> int {
  return std::stoi(string.data());
}

template <typename T, typename Iterator>
[[nodiscard]] auto get_parameter_from_args_as(Iterator begin, Iterator end, std::string parameter)
    -> std::optional<T> try {
  Iterator it = std::find(begin, end, parameter);
  if (it != end && ++it != end) {
    return cast_to<T>(*it);
  }
  return std::nullopt;
} catch (std::exception const& exception) {
  std::cerr << "Unexpected exception caught with error message " << exception.what();
  return std::nullopt;
}

int _main(std::span<const std::string_view> const args) {
  std::optional<int> const port = get_parameter_from_args_as<int>(args.begin(), args.end(), "--port");
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
            "text/plain"
        );
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

  server.Get("/ping", [](const httplib::Request&, httplib::Response&) {});

  server.listen("0.0.0.0", port.value());
  executor->join();

  return 0;
}

}  // namespace

int main(int const argc, char const* const* argv) {
  std::vector<std::string_view> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));

  return _main(args);
}