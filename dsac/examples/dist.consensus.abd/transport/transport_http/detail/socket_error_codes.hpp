#pragma once

#include <system_error>

namespace dsac {

enum class socket_status : char {
  too_many_open_files,
  cant_accept_socket,
};

}  // namespace dsac

namespace std {

template <>
struct is_error_code_enum<dsac::socket_status> : true_type {};

}  // namespace std
