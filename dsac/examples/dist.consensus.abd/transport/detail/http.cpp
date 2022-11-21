#include <examples/dist.consensus.abd/transport/detail/http.hpp>

#include <dsac/functional/expected.hpp>

#include <atomic>
#include <memory>

#include <netdb.h>

namespace dsac {

namespace {

constexpr const char* kEndpoint                    = "0.0.0.0";
constexpr const int   kInvalidSocket               = -1;
constexpr const int   kNumberOfConnectionsOnSocket = 5;

dsac::expected<int, std::string> create_server_socket(std::string const& endpoint, std::string const& port) {
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
    return dsac::make_unexpected("cant get a list of address");
  }

  std::shared_ptr<addrinfo> addrinfo_defer{service_provider, [](addrinfo* addrinfo) { freeaddrinfo(addrinfo); }};
  for (addrinfo* provider = service_provider; provider != nullptr; provider = provider->ai_next) {
    int const socket = ::socket(provider->ai_family, provider->ai_socktype, provider->ai_protocol);
    if (socket == kInvalidSocket) {
      continue;
    }

    if (::bind(socket, provider->ai_addr, static_cast<socklen_t>(provider->ai_addrlen)) != 0) {
      return dsac::make_unexpected("cant bind to the socket");
    }

    if (::listen(socket, kNumberOfConnectionsOnSocket) != 0) {
      return dsac::make_unexpected("cant listen on the socket");
    }

    return socket;
  }

  return dsac::make_unexpected("cant search service provider");
}

}  // namespace

class transport_http::transport_http_pimpl {
  std::atomic<int> socket_;

public:
  void serve(int port) {
    const dsac::expected<int, std::string> socket = create_server_socket(kEndpoint, std::to_string(port));
    if (!socket.has_value()) {
      throw transport_exception{socket.error()};
    }

    socket_.store(socket.value());
  }
};

transport_http::transport_http()
  : pimpl(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl->serve(port);
}

}  // namespace dsac
