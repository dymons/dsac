#include <examples/dist.consensus.abd/transport/detail/http.hpp>

#include <dsac/functional/expected.hpp>

#include <atomic>
#include <memory>

#include <netdb.h>

namespace dsac {

namespace {

constexpr const char* kEndpoint = "0.0.0.0";

}  // namespace

class transport_http::transport_http_pimpl {
  std::atomic<int> socket_;

public:
  void serve(int port) {
    const dsac::expected<int, std::string> socket = create_server_socket(kEndpoint, std::to_string(port));
  }

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

    return 1;
  }
};

transport_http::transport_http()
  : pimpl(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl->serve(port);
}

}  // namespace dsac
