#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace {
constexpr const int kPort = 5555;
constexpr const int kBufferSize = 512;

using Descriptor = int;
using Message = std::string;
using MessageView = std::string_view;

enum class Status : unsigned char { Ok, Error };
Status sent_message(Descriptor descriptor, Message message) {
  std::reverse(message.begin(), message.end());

  std::cout << "Descriptor: " << descriptor << ", Message " << message << std::endl;
  int const bytes = write(descriptor, message.data(), message.size());
  if (bytes < 0) {
    return Status::Error;
  }

  return Status::Ok;
}

struct Response {
  Status status;
  Message message;
};
Response receive_message(Descriptor descriptor) {
  Message message(kBufferSize, '0');
  int const bytes = read(descriptor, message.data(), kBufferSize);
  if (bytes < 0) {
    return {Status::Error};
  } else {
    message.resize(bytes);
    message.shrink_to_fit();

    if (bytes == 0) {
      std::cout << "Descriptor: " << descriptor << ", not data from client" << std::endl;
      return {Status::Error};
    } else {
      std::cout << "Descriptor: " << descriptor << ", Message " << message << std::endl;
    }
  }

  return {Status::Ok, std::move(message)};
}
}  // namespace

int main() {
  // Create a connection description
  int opt = 1;
  constexpr const int kUseTcp = 0;
  Descriptor const descriptor = socket(PF_INET, SOCK_STREAM, kUseTcp);
  if (descriptor < 0) {
    std::cout << "Socket was not created" << std::endl;
    return 1;
  }
  setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

  // Start listening on the port
  sockaddr_in socket_description;
  socket_description.sin_family = AF_INET;
  socket_description.sin_port = htons(kPort);
  socket_description.sin_addr.s_addr = htonl(INADDR_ANY);
  if (int const error =
          bind(descriptor, (sockaddr*)(&socket_description), sizeof(socket_description));
      error < 0) {
    std::cout << "Failed to bind a socket" << std::endl;
    return 1;
  }

  // Create a queue for request from clients
  constexpr const int kSizeQueue = 3;
  if (int const error = listen(descriptor, kSizeQueue); error < 0) {
    std::cout << "Listen queue failure" << std::endl;
    return 1;
  }

  // Init active fd
  fd_set active_set;
  FD_ZERO(&active_set);
  FD_SET(descriptor, &active_set);

  // Handling request from client
  std::cout << "Start handling request from client on port " << kPort << std::endl;
  sockaddr_in client_socket_description;
  while (true) {
    fd_set current_set = active_set;
    if (select(FD_SETSIZE, &current_set, nullptr, nullptr, nullptr) < 0) {
      std::cout << "Select failure" << std::endl;
      return 1;
    }

    for (int i{}; i < FD_SETSIZE; ++i) {
      if (!FD_ISSET(i, &current_set)) {
        continue;
      }

      if (i == descriptor) {
        socklen_t size = sizeof(client_socket_description);
        Descriptor new_descriptor =
            accept(descriptor, (sockaddr*)(&client_socket_description), &size);
        if (new_descriptor < 0) {
          std::cout << "Socket was not created" << std::endl;
          return 1;
        }

        std::cout << "New connection on Port: "
                  << ntohs(client_socket_description.sin_port) << std::endl;
        FD_SET(new_descriptor, &active_set);
      } else {
        Response const response = receive_message(i);
        if (response.status == Status::Error) {
          close(i);
          FD_CLR(i, &active_set);
          std::cout << "Descriptor: " << i << ", Stop listening" << std::endl;
        } else {
          if (response.message == "stop") {
            close(i);
            FD_CLR(i, &active_set);
            std::cout << "Descriptor: " << i << ", Stop listening" << std::endl;
          } else {
            sent_message(i, response.message);
          }
        }
      }
    }
  }

  close(descriptor);
  return 0;
}