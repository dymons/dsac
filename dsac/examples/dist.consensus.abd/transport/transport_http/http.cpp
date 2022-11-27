#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/http.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <atomic>
#include <chrono>
#include <thread>

namespace dsac {

namespace {

using namespace std::chrono_literals;

constexpr const char*               kEndpoint                 = "0.0.0.0";
constexpr std::size_t               kKeepAliveMaxCount        = 5U;
constexpr std::chrono::seconds      kKeepAliveTimeoutSeconds  = 5s;
constexpr std::chrono::seconds      kReadTimeoutSeconds       = 5s;
constexpr std::chrono::milliseconds kReadTimeoutMilliseconds  = 0ms;
constexpr std::chrono::seconds      kWriteTimeoutSeconds      = 5s;
constexpr std::chrono::milliseconds kWriteTimeoutMilliseconds = 0ms;
constexpr std::chrono::seconds      kIdleTimeoutSeconds       = 5s;
constexpr std::chrono::milliseconds kIdleTimeoutMilliseconds  = 0ms;
constexpr std::size_t               kPayloadMaxSize           = std::numeric_limits<std::size_t>::max();

}  // namespace

class transport_http::transport_http_pimpl {
  std::atomic<int>        server_socket_;
  dsac::executor_base_ref executor_ = dsac::make_static_thread_pool(std::thread::hardware_concurrency());

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

  auto process_socket_at_executor() {
    return [this](dsac::expected<int, socket_status> const& socket) -> void {
      if (socket.has_value()) {
        executor_->submit([socket = socket.value(), this]() { process_and_close_socket(socket); });
      }
    };
  }

  auto serve_server_on_socket() {
    return [this](dsac::expected<int, socket_status> const& server_socket) -> dsac::expected<int, socket_status> {
      server_socket_.store(server_socket.value());
      while (is_socket_valid(server_socket_)) {
        accept_server_socket(server_socket_)
            .and_then(check_peer_socket_status())
            .and_then(close_server_socket_if_error())
            .map(process_socket_at_executor());
      }
      return server_socket_.load();
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

  auto await_stop_server() {
    return [this]([[maybe_unused]] auto&&) -> void { executor_->join(); };
  }

  void process_and_close_socket(int socket) {
  }

public:
  void serve(int port) {
    create_server_socket(kEndpoint, std::to_string(port))
        .and_then(check_server_socket_status())
        .and_then(serve_server_on_socket())
        .map(await_stop_server());
  }
};

transport_http::transport_http()
  : pimpl_(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl_->serve(port);
}

}  // namespace dsac
