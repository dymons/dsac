#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/transport_http.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <atomic>
#include <chrono>
#include <stop_token>
#include <thread>

namespace dsac {

namespace {

using namespace std::chrono_literals;

constexpr const char*          kEndpoint                = "0.0.0.0";
const std::size_t              kWorkersCount            = std::thread::hardware_concurrency();
constexpr std::size_t          kKeepAliveMaxCount       = 5UL;
constexpr std::chrono::seconds kKeepAliveTimeoutSeconds = 5s;

auto keep_alive_peer_socket(std::stop_token const& stop_token, int const socket) -> bool {
  std::size_t keep_alive_count_attempts = std::max(kKeepAliveMaxCount, 1UL);
  while (!stop_token.stop_requested() && keep_alive_count_attempts-- != 0UL) {
    if (keep_alive(socket, kKeepAliveTimeoutSeconds)) {
      return true;
    }
  }
  return false;
}

void process_peer_socket(std::stop_token&& stop_token, int const socket) {
  if (!keep_alive_peer_socket(stop_token, socket)) {
    return;
  }
}

void process_and_close_socket(std::stop_token&& stop_token, int const socket) {
  process_peer_socket(std::move(stop_token), socket);
  shutdown_socket(socket);
  close_socket(socket);
}

auto check_peer_socket_status() {
  return [](dsac::expected<int, socket_status> const& socket) -> dsac::expected<int, socket_status> {
    if (!socket.has_value()) {
      if (socket.error() == socket_status::too_many_open_files) {
        return socket;
      }

      return dsac::make_unexpected(socket_status::closed_by_user);
    }

    return socket;
  };
}

auto close_server_socket_if_error(int const server_socket) {
  return [server_socket = server_socket](
             dsac::expected<int, socket_status> const& peer_socket) -> dsac::expected<int, socket_status> {
    if (!peer_socket.has_value() && peer_socket.error() != socket_status::too_many_open_files) {
      if (!is_socket_valid(server_socket)) {
        close_socket(server_socket);
      }

      return dsac::make_unexpected(socket_status::closed_by_user);
    }

    return peer_socket;
  };
}

auto process_socket_at_executor(dsac::executor_base_ref executor, std::stop_token stop_token) {
  return [executor   = std::move(executor),
          stop_token = std::move(stop_token)](dsac::expected<int, socket_status> const& socket) mutable -> void {
    if (socket.has_value()) {
      executor->submit([socket = socket.value(), stop_token = std::move(stop_token)]() mutable {
        process_and_close_socket(std::move(stop_token), socket);
      });
    }
  };
}

auto check_server_socket_status() {
  return [](dsac::expected<int, socket_status> const& socket) -> dsac::expected<int, socket_status> {
    if (!socket.has_value()) {
      throw transport_server_socket{to_string(socket.error())};
    }

    return socket;
  };
}

auto serve_server_on_executor(dsac::executor_base_ref&& executor) {
  return [executor = std::move(executor)](dsac::expected<int, socket_status> const& server_socket) mutable
         -> dsac::expected<dsac::executor_base_ref, socket_status> {
    std::stop_source stop_source;
    while (is_socket_readable(server_socket.value())) {
      accept_server_socket(server_socket.value())
          .and_then(check_peer_socket_status())
          .and_then(close_server_socket_if_error(server_socket.value()))
          .map(process_socket_at_executor(executor, stop_source.get_token()));
    }
    stop_source.request_stop();
    return std::move(executor);
  };
}

auto await_stop_serve_server() {
  return [](dsac::executor_base_ref&& executor) -> void { executor->join(); };
}

}  // namespace

void transport_http::serve(int port) {
  create_server_socket(kEndpoint, std::to_string(port))
      .and_then(check_server_socket_status())
      .and_then(serve_server_on_executor(make_static_thread_pool(kWorkersCount)))
      .map(await_stop_serve_server());
}

}  // namespace dsac
