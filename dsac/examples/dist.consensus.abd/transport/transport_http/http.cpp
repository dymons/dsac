#include <examples/dist.consensus.abd/transport/transport_http/detail/socket.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/http.hpp>

#include <atomic>

namespace dsac {

namespace {

constexpr const char* kEndpoint = "0.0.0.0";

}  // namespace

class transport_http::transport_http_pimpl {
  std::atomic<int> server_socket_;

public:
  void serve(int port) {
    const dsac::expected<int, std::string> server_socket = create_server_socket(kEndpoint, std::to_string(port));
    if (!server_socket.has_value()) {
      throw transport_exception{server_socket.error()};
    }

    server_socket_.store(server_socket.value());
  }
};

transport_http::transport_http()
  : pimpl(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl->serve(port);
}

}  // namespace dsac
