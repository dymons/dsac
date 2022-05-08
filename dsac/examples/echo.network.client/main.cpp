#include <iostream>
#include <string>
#include <string_view>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fmt/format.h>

namespace {

constexpr const int              kPort       = 5555;
constexpr const int              kBufferSize = 512;
constexpr const std::string_view kEndpoint   = "127.0.0.1";

using descriptor   = int;
using message      = std::string;
using message_view = std::string_view;

enum class status : unsigned char { ok, error, unknown };
status sent_message(descriptor const descriptor, message_view const view) {
  long const bytes = write(descriptor, view.data(), view.size());
  return bytes < 0 ? status::error : status::ok;
}

struct response {
  status  code = status::unknown;
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
}  // namespace

int main() {
  hostent const* const hostinfo = gethostbyname(kEndpoint.data());
  if (hostinfo == nullptr) {
    std::cout << "unknown endpoint server " << kEndpoint << std::endl;
    return 1;
  }

  sockaddr_in socket_desc{};
  socket_desc.sin_family      = AF_INET;
  socket_desc.sin_port        = htons(kPort);
  socket_desc.sin_addr.s_addr = inet_addr("0.0.0.0");

  constexpr const int kUseTcp     = 0;
  descriptor const    remote_desc = socket(AF_INET, SOCK_STREAM, kUseTcp);
  if (remote_desc < 0) {
    std::cout << "socket was not created" << std::endl;
    return 1;
  }

  int const error = connect(remote_desc, reinterpret_cast<sockaddr*>(&socket_desc), sizeof(socket_desc));
  if (error < 0) {
    std::cout << "failed to connect to the server" << std::endl;
    return 1;
  }

  while (true) {
    std::cout << "\n> ";
    std::string message;
    std::cin >> message;
    if (status const status = sent_message(remote_desc, message); status == status::error) {
      break;
    }

    response const res = receive_message(remote_desc);
    if (res.code == status::error) {
      break;
    }
    std::cout << fmt::format("send message '{}', receive message '{}'", message, res.payload) << std::endl;
  }

  close(remote_desc);

  return 0;
}