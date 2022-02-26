#include <iostream>
#include <string>
#include <string_view>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <fmt/format.h>

namespace {
constexpr const int kPort = 5555;
constexpr const int kBufferSize = 512;
constexpr const std::string_view kEndpoint = "127.0.0.1";

using Descriptor = int;
using Message = std::string;
using MessageView = std::string_view;

enum class Status : unsigned char { Ok, Error };
Status SentMessage(Descriptor desc, MessageView msg) {
  std::cout << fmt::format("Sent message \"{}\" to desc {}", msg, desc) << std::endl;
  int const bytes = write(desc, msg.data(), msg.size());
  if (bytes < 0 || msg == "stop") {
    return Status::Error;
  }

  return Status::Ok;
}

struct Response {
  Status status;
  Message message;
};
Response ReceiveMessage(Descriptor desc) {
  Message msg(kBufferSize, '0');
  int const bytes = read(desc, msg.data(), kBufferSize);
  if (bytes < 0) {
    return {Status::Error};
  } else {
    msg.resize(bytes);
    std::cout << fmt::format("Receive msg \"{}\" from desc {}", msg, desc) << std::endl;
  }

  return {Status::Ok, std::move(msg)};
}
}  // namespace

int main() {
  hostent const* const hostinfo = gethostbyname(kEndpoint.data());
  if (hostinfo == nullptr) {
    std::cout << "Unknown endpoint server " << kEndpoint << std::endl;
    return 1;
  }

  sockaddr_in socket_desc;
  socket_desc.sin_family = AF_INET;
  socket_desc.sin_port = htons(kPort);
  socket_desc.sin_addr.s_addr = inet_addr("0.0.0.0");

  constexpr const int kUseTcp = 0;
  Descriptor const remote_desc = socket(AF_INET, SOCK_STREAM, kUseTcp);
  if (remote_desc < 0) {
    std::cout << "Socket was not created" << std::endl;
    return 1;
  }

  int const error = connect(remote_desc, (sockaddr*)(&socket_desc), sizeof(socket_desc));
  if (error < 0) {
    std::cout << "Failed to connect to the server" << std::endl;
    return 1;
  }

  while (true) {
    std::cout << "\nEnter a new message: > ";
    std::string message;
    std::cin >> message;
    if (Status status = SentMessage(remote_desc, message); status == Status::Error) {
      break;
    }

    if (Response res = ReceiveMessage(remote_desc); res.status == Status::Error) {
      break;
    }
  }

  close(remote_desc);

  return 0;
}