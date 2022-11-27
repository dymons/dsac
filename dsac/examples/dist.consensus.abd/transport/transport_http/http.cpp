#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/http.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <atomic>
#include <stop_token>
#include <thread>

namespace dsac {

namespace {

constexpr const char* kEndpoint       = "0.0.0.0";
constexpr std::size_t kPayloadMaxSize = std::numeric_limits<std::size_t>::max();
const std::size_t     kWorkersCount   = std::thread::hardware_concurrency();

}  // namespace

class transport_http::transport_http_pimpl {
  std::atomic<int> server_socket_;

  auto check_peer_socket_status() {
    return [this](dsac::expected<int, socket_status> const& socket) -> dsac::expected<int, socket_status> {
      if (!socket.has_value()) {
        if (socket.error() == socket_status::too_many_open_files) {
          return socket;
        }

        return dsac::make_unexpected(socket_status::closed_by_user);
      }

      return socket;
    };
  }

  auto close_server_socket_if_error() {
    return [this](dsac::expected<int, socket_status> const& peer_socket) -> dsac::expected<int, socket_status> {
      if (!peer_socket.has_value() && peer_socket.error() != socket_status::too_many_open_files) {
        if (!is_socket_valid(server_socket_)) {
          close_socket(server_socket_);
        }

        return dsac::make_unexpected(socket_status::closed_by_user);
      }

      return peer_socket;
    };
  }

  auto process_socket_at_executor(dsac::executor_base_ref executor, std::stop_token stop_token) {
    return [executor = std::move(executor), stop_token = std::move(stop_token), this](
               dsac::expected<int, socket_status> const& socket) mutable -> void {
      if (socket.has_value()) {
        executor->submit([socket = socket.value(), stop_token = std::move(stop_token), this]() {
          process_and_close_socket(stop_token, socket);
        });
      }
    };
  }

  auto check_server_socket_status() {
    return [this](dsac::expected<int, socket_status> const& socket) -> dsac::expected<int, socket_status> {
      if (!socket.has_value()) {
        throw transport_server_socket{to_string(socket.error())};
      }

      return socket;
    };
  }

  auto serve_server_on_executor(dsac::executor_base_ref&& executor) {
    return [executor = std::move(executor), this](dsac::expected<int, socket_status> const& socket) mutable
           -> dsac::expected<dsac::executor_base_ref, socket_status> {
      std::stop_source stop_source;
      while (is_socket_readable(socket.value())) {
        accept_server_socket(socket.value())
            .and_then(check_peer_socket_status())
            .and_then(close_server_socket_if_error())
            .map(process_socket_at_executor(executor, stop_source.get_token()));
      }
      stop_source.request_stop();
      return std::move(executor);
    };
  }

  auto await_stop_serve_server() {
    return [this](dsac::executor_base_ref&& executor) -> void { executor->join(); };
  }

  void process_and_close_socket(std::stop_token stop_token, int socket) {
  }

public:
  void serve(int const port) {
    create_server_socket(kEndpoint, std::to_string(port))
        .and_then(check_server_socket_status())
        .and_then(serve_server_on_executor(make_static_thread_pool(kWorkersCount)))
        .map(await_stop_serve_server());
  }
};

transport_http::transport_http()
  : pimpl_(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl_->serve(port);
}

}  // namespace dsac
