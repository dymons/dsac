#pragma once

#include <examples/dist.consensus.abd/transport/transport_http/detail/socket_error_codes.hpp>

#include <dsac/functional/expected.hpp>

#include <chrono>
#include <string>

namespace dsac {

auto create_server_socket(std::string const& endpoint, std::string const& port) -> dsac::expected<int, socket_status>;

auto accept_server_socket(int server_socket) -> dsac::expected<int, socket_status>;

auto is_socket_valid(int socket) -> bool;

auto is_socket_readable(int socket) -> bool;

auto close_socket(int socket) -> void;

auto shutdown_socket(int socket) -> void;

auto keep_alive(int socket, std::chrono::seconds timeout) -> bool;

}  // namespace dsac
