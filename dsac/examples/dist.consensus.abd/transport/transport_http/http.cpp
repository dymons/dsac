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

  void serve_impl() {
    while (is_socket_valid(server_socket_)) {
      dsac::expected<int, socket_status> const socket = accept_server_socket(server_socket_);
      if (!socket.has_value()) {
        if (socket.error() == socket_status::too_many_open_files) {
          continue;
        }

        if (!is_socket_valid(server_socket_)) {
          close_socket(server_socket_);
        }

        break;
      }

      executor_->submit([socket = socket.value(), this]() { process_and_close_socket(socket); });
    }

    executor_->join();
  }

  void process_and_close_socket(int socket) {
  }

public:
  void serve(int port) {
    dsac::expected<int, std::string> const server_socket = create_server_socket(kEndpoint, std::to_string(port));
    if (!server_socket.has_value()) {
      throw transport_exception{server_socket.error()};
    }

    serve_impl();
  }
};

transport_http::transport_http()
  : pimpl_(make_shared<transport_http_pimpl>()) {
}

void transport_http::serve(int port) {
  pimpl_->serve(port);
}

}  // namespace dsac
