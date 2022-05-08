#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr const int kPort       = 5555;
constexpr const int kBufferSize = 512;

using descriptor   = int;
using message      = std::string;
using message_view = std::string_view;

enum class status : unsigned char { ok, error };
status sent_message(descriptor descriptor, message_view const message) {
  long const bytes = write(descriptor, message.data(), message.size());
  return bytes < 0 ? status::error : status::ok;
}

struct response {
  status  code;
  message payload;
};
response receive_message(descriptor descriptor) {
  response   response{.code = status::ok, .payload = message(kBufferSize, '0')};
  long const bytes = read(descriptor, response.payload.data(), response.payload.size());
  if (bytes < 0) {
    response.code = status::error;
    response.payload.clear();
    response.payload.shrink_to_fit();
    return response;
  }

  response.payload.resize(bytes);
  return response;
}

[[noreturn]] void start_handling_requests_on_socket(descriptor const descriptor_listener) {
  fd_set active_set;
  FD_ZERO(&active_set);
  FD_SET(descriptor_listener, &active_set);

  while (true) {
    fd_set snapshot_set = active_set;
    if (select(FD_SETSIZE, &snapshot_set, nullptr, nullptr, nullptr) < 0) {
      std::cout << "select failure" << std::endl;
      continue;
    }

    for (int i{}; i < FD_SETSIZE; ++i) {
      if (!FD_ISSET(i, &snapshot_set)) {
        continue;
      }

      if (i == descriptor_listener) {
        sockaddr_in client_socket_description{};
        socklen_t   size = sizeof(client_socket_description);
        descriptor  new_connection_descriptor =
            accept(descriptor_listener, reinterpret_cast<sockaddr*>(&client_socket_description), &size);
        if (new_connection_descriptor < 0) {
          std::cout << "socket was not created" << std::endl;
          continue;
        }

        FD_SET(new_connection_descriptor, &active_set);
      } else {
        response const response = receive_message(i);
        if (response.code == status::error) {
          close(i);
          FD_CLR(i, &active_set);
        } else {
          sent_message(i, response.payload);
        }
      }
    }
  }
}
}  // namespace

int main() {
  constexpr const int kUseTcp       = 0;
  descriptor const    desc_listener = socket(PF_INET, SOCK_STREAM, kUseTcp);
  if (desc_listener < 0) {
    std::cout << "socket listener was not created" << std::endl;
    return 1;
  }

  int opt = 1;
  setsockopt(desc_listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt));

  sockaddr_in sock_description{};
  sock_description.sin_family      = AF_INET;
  sock_description.sin_port        = htons(kPort);
  sock_description.sin_addr.s_addr = htonl(INADDR_ANY);
  int const error_bind = bind(desc_listener, reinterpret_cast<sockaddr*>(&sock_description), sizeof(sock_description));
  if (error_bind < 0) {
    std::cout << "Failed to bind a socket" << std::endl;
    return 1;
  }

  constexpr const int kSizeQueue = 3;
  if (int const error = listen(desc_listener, kSizeQueue); error < 0) {
    std::cout << "Listen queue failure" << std::endl;
    return 1;
  }

  start_handling_requests_on_socket(desc_listener);
}
