#pragma once

#include <dsac/functional/expected.hpp>

#include <string>

namespace dsac {

auto create_server_socket(std::string const& endpoint, std::string const& port) -> dsac::expected<int, std::string>;

} // namespace dsac
