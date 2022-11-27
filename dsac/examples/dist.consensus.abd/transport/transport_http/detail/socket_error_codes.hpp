#pragma once

#include <string>
#include <system_error>

namespace dsac {

enum class socket_status : char {
  too_many_open_files,
  cant_accept_socket,
  closed_by_user,
  not_found_address_info,
  not_found_service_provider,
  cant_bind_socket,
  cant_listen_socket,
};

std::string to_string(socket_status status);

}  // namespace dsac

namespace std {

template <>
struct is_error_code_enum<dsac::socket_status> : true_type {};

}  // namespace std
