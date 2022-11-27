#include <examples/dist.consensus.abd/transport/transport_http/detail/socket_error_codes.hpp>

namespace dsac {

std::string to_string(socket_status status) {
  switch (status) {
    case socket_status::too_many_open_files:
      return "socket_status::too_many_open_files";
    case socket_status::cant_accept_socket:
      return "socket_status::cant_accept_socket";
    case socket_status::closed_by_user:
      return "socket_status::closed_by_user";
    case socket_status::not_found_address_info:
      return "socket_status::not_found_address_info";
    case socket_status::not_found_service_provider:
      return "socket_status::not_found_service_provider";
    case socket_status::cant_bind_socket:
      return "socket_status::cant_bind_socket";
    case socket_status::cant_listen_socket:
      return "socket_status::cant_listen_socket";
  }
  return "socket_status::unexpected_socket_error";
}

}  // namespace dsac
