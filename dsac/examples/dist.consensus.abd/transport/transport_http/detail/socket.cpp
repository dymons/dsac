#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>

#include <chrono>
#include <memory>

#include <netdb.h>
#include <poll.h>

namespace dsac {

namespace {

using namespace std::chrono_literals;

constexpr const inline int          kInvalidSocket               = -1;
constexpr const int                 kNumberOfConnectionsOnSocket = 5;
constexpr std::chrono::seconds      kReadTimeoutSeconds          = 5s;
constexpr std::chrono::milliseconds kReadTimeoutMilliseconds     = 0ms;
constexpr std::chrono::seconds      kWriteTimeoutSeconds         = 5s;
constexpr std::chrono::milliseconds kWriteTimeoutMilliseconds    = 0ms;
constexpr std::chrono::seconds      kIdleTimeoutSeconds          = 5s;
constexpr std::chrono::milliseconds kIdleTimeoutMilliseconds     = 0ms;

inline auto handle_with_retry_on_eintr_signal(auto&& routine) -> ssize_t {
  while (true) {
    if (auto res = routine(); !(res < 0 && errno == EINTR)) {
      return res;
    }
  }
  return 0;
}

[[nodiscard]] bool is_readable(
    int const                 socket,
    std::chrono::seconds      timeout_read_seconds      = kReadTimeoutSeconds,
    std::chrono::milliseconds timeout_read_milliseconds = kReadTimeoutMilliseconds) {
  pollfd pfd_read{
      .fd     = socket,
      .events = POLLIN,
  };

  int constexpr kNumberOfFileDescriptors = 1;
  int const timeout = static_cast<int>(timeout_read_seconds.count() * 1000 + timeout_read_milliseconds.count() / 1000);
  return handle_with_retry_on_eintr_signal([&]() { return poll(&pfd_read, kNumberOfFileDescriptors, timeout); }) > 0;
}

}  // namespace

auto create_server_socket(std::string const& endpoint, std::string const& port) -> dsac::expected<int, socket_status> {
  addrinfo hints{
      .ai_flags     = 0,
      .ai_family    = 0,
      .ai_socktype  = SOCK_STREAM,
      .ai_protocol  = 0,
      .ai_addrlen   = 0,
      .ai_addr      = nullptr,
      .ai_canonname = nullptr,
      .ai_next      = nullptr,
  };

  addrinfo* service_provider = nullptr;
  if (getaddrinfo(endpoint.c_str(), port.c_str(), &hints, &service_provider) != 0) {
    return dsac::make_unexpected(socket_status::not_found_address_info);
  }

  std::shared_ptr<addrinfo> addrinfo_defer{service_provider, [](addrinfo* addrinfo) { freeaddrinfo(addrinfo); }};
  for (addrinfo* provider = service_provider; provider != nullptr; provider = provider->ai_next) {
    int const socket = ::socket(provider->ai_family, provider->ai_socktype, provider->ai_protocol);
    if (socket == kInvalidSocket) {
      continue;
    }

    if (::bind(socket, provider->ai_addr, static_cast<socklen_t>(provider->ai_addrlen)) != 0) {
      return dsac::make_unexpected(socket_status::cant_bind_socket);
    }

    if (::listen(socket, kNumberOfConnectionsOnSocket) != 0) {
      return dsac::make_unexpected(socket_status::cant_listen_socket);
    }

    return socket;
  }

  return dsac::make_unexpected(socket_status::not_found_service_provider);
}

auto accept_server_socket(int server_socket) -> dsac::expected<int, socket_status> {
  int const socket = ::accept(server_socket, nullptr, nullptr);
  if (socket == kInvalidSocket) {
    if (errno == EMFILE) {
      return dsac::make_unexpected(dsac::socket_status::too_many_open_files);
    }
    return dsac::make_unexpected(dsac::socket_status::cant_accept_socket);
  }
  return socket;
}

auto is_socket_valid(int socket) -> bool {
  return socket != kInvalidSocket;
}

auto is_socket_readable(int socket) -> bool {
  return is_socket_valid(socket) && is_readable(socket);
}

auto close_socket(int socket) -> void {
  ::close(socket);
}

auto shutdown_socket(int socket) -> void {
  ::shutdown(socket, SHUT_RDWR);
}

auto keep_alive(int const socket, std::chrono::seconds const timeout) -> bool {
  auto const start_keep_alive = std::chrono::steady_clock::now();
  while (!is_readable(socket, timeout)) {
    auto const current_keep_alive  = std::chrono::steady_clock::now();
    auto const duration_keep_alive = duration_cast<std::chrono::milliseconds>(current_keep_alive - start_keep_alive);
    if (duration_keep_alive.count() > duration_cast<std::chrono::milliseconds>(timeout).count()) {
      return false;
    }
  }
  return true;
}

auto read_socket(int socket, void* ptr, std::size_t size, int flags) -> ssize_t {
  return handle_with_retry_on_eintr_signal([&]() { return recv(socket, ptr, size, flags); });
}

}  // namespace dsac