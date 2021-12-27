#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

namespace {
constexpr const int kPort = 5555;
constexpr const int kBufferSize = 512;
constexpr const std::string_view kEndpoint = "127.0.0.1";

using Descriptor = int;
using Message = std::string;
using MessageView = std::string_view;

enum class Status : unsigned char { Ok, Error };
Status sent_message(Descriptor descriptor, MessageView message) {
  std::cout << "Descriptor: " << descriptor << ", Message " << message << std::endl;
  int const bytes = write(descriptor, message.data(), message.size());
  if (bytes < 0 || message == "stop") {
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
      std::cout << "Descriptor: " << descriptor << ", not data from server" << std::endl;
    } else {
      std::cout << "Descriptor: " << descriptor << ", Message: " << message << std::endl;
    }
  }

  return {Status::Ok, std::move(message)};
}
}  // namespace

int main() {
  /*
   * Получаем информацию о сервере по его DNS имени/IP адресу
   */
  hostent const* hostinfo = gethostbyname(kEndpoint.data());
  if (hostinfo == nullptr) {
    std::cout << "Unknown endpoint server " << kEndpoint << std::endl;
    return 1;
  }

  /*
   * Инициализация структуры для последующего соединения с сервером
   */
  sockaddr_in socket_description;
  socket_description.sin_family = AF_INET;
  socket_description.sin_port = htons(kPort);
  socket_description.sin_addr.s_addr = inet_addr("127.0.0.1");

  /*
   * Создаем TCP сокет
   */
  constexpr const int kUseTcp = 0;
  Descriptor descriptor = socket(AF_INET, SOCK_STREAM, kUseTcp);
  if (descriptor < 0) {
    std::cout << "Socket was not created" << std::endl;
    return 1;
  }

  /*
   * Устанавливаем соединение с сервером
   */
  int const error =
      connect(descriptor, (sockaddr*)(&socket_description), sizeof(socket_description));
  if (error < 0) {
    std::cout << "Failed to connect to the server" << std::endl;
    return 1;
  }

  while (true) {
    std::cout << "\nEnter a new message: > ";
    std::string message;
    std::cin >> message;
    if (Status status = sent_message(descriptor, message); status == Status::Error) {
      break;
    }

    if (Response res = receive_message(descriptor); res.status == Status::Error) {
      break;
    }
  }

  /*
   * Закрываем соединение с сервером
   */
  close(descriptor);

  return 0;
}