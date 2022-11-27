#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/transport_http.hpp>

#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <atomic>
#include <chrono>
#include <cstring>
#include <optional>
#include <ranges>
#include <stop_token>
#include <string_view>
#include <thread>

namespace dsac {

namespace {

using namespace std::chrono_literals;

constexpr const char*          kEndpoint                = "0.0.0.0";
const std::size_t              kWorkersCount            = std::thread::hardware_concurrency();
constexpr std::size_t          kKeepAliveMaxCount       = 5UL;
constexpr std::chrono::seconds kKeepAliveTimeoutSeconds = 5s;
constexpr std::size_t          kMaxBufferSize           = 2048;

class socket_stream final {
  const int         socket_;
  std::vector<char> buffer_;
  ssize_t           buffer_head_{0UL};

public:
  explicit socket_stream(int const socket)
    : socket_(socket) {
  }

  ssize_t read(char* content, ssize_t const requested_content_size) {
    if (buffer_head_ < buffer_.size()) {
      ssize_t const remaining_size = static_cast<ssize_t>(buffer_.size()) - buffer_head_;
      if (requested_content_size <= remaining_size) {
        std::memcpy(content, std::next(buffer_.data(), buffer_head_), requested_content_size);
        buffer_head_ += requested_content_size;
        return static_cast<ssize_t>(requested_content_size);
      }

      std::memcpy(content, std::next(buffer_.data(), buffer_head_), remaining_size);
      buffer_head_ += remaining_size;
      return static_cast<ssize_t>(remaining_size);
    }

    if (!dsac::is_socket_readable(socket_)) {
      return -1;
    }

    if (requested_content_size < kMaxBufferSize) {
      buffer_.resize(kMaxBufferSize, '0');
      auto n = read_socket(socket_, buffer_.data(), buffer_.size(), 0);
      if (n <= 0) {
        return n;
      }

      buffer_head_ = 0UL;

      if (n <= requested_content_size) {
        std::memcpy(content, buffer_.data(), static_cast<size_t>(n));
        return n;
      }

      memcpy(content, buffer_.data(), requested_content_size);
      buffer_head_ = requested_content_size;
      buffer_.resize(static_cast<size_t>(n));
      return static_cast<ssize_t>(requested_content_size);
    }

    return read_socket(socket_, content, requested_content_size, 0);
  }
};

class socket_parser final {
  socket_stream socket_stream_;

public:
  socket_parser(socket_stream&& socket_stream)  // NOLINT(google-explicit-constructor)
    : socket_stream_(std::move(socket_stream)) {
  }

  std::optional<std::string> get_next_line() {
    std::string next_line;

    while (true) {
      char          byte = '0';
      ssize_t const n    = socket_stream_.read(&byte, 1);
      if (n < 0) {
        return std::nullopt;
      }
      if (n == 0) {
        break;
      }

      if (next_line += byte; byte == '\n') {
        break;
      }
    }

    return next_line;
  }
};

auto keep_alive_peer_socket(std::stop_token const& stop_token, int const socket) -> bool {
  std::size_t keep_alive_count_attempts = std::max(kKeepAliveMaxCount, 1UL);
  while (!stop_token.stop_requested() && keep_alive_count_attempts-- != 0UL) {
    if (keep_alive(socket, kKeepAliveTimeoutSeconds)) {
      return true;
    }
  }
  return false;
}

auto get_target_method(std::string const& start_query) -> std::string {
  std::string_view const view_query{start_query};
  std::string_view const view_space{" "};

  for (auto target : view_query | std::ranges::views::split(view_space) | std::ranges::views::drop(1)) {
    return std::string(&*target.begin(), std::ranges::distance(target));
  }

  return std::string{};
}

void process_peer_socket(std::stop_token&& stop_token, int const socket) {
  if (!keep_alive_peer_socket(stop_token, socket)) {
    return;
  }

  socket_parser                    socket_parser = socket_stream{socket};
  const std::optional<std::string> start_query   = socket_parser.get_next_line();
  if (!start_query.has_value()) {
    return;
  }

  request request;
  request.method = get_target_method(start_query.value());
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
