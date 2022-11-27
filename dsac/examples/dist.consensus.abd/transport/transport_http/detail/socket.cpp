#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>

#include <memory>

#include <netdb.h>

namespace dsac {

namespace {

constexpr const inline int kInvalidSocket               = -1;
constexpr const int        kNumberOfConnectionsOnSocket = 5;

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
    if (is_socket_valid(socket)) {
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
  if (!is_socket_valid(socket)) {
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

auto close_socket(int socket) -> void {
  ::close(socket);
}

}  // namespace dsac