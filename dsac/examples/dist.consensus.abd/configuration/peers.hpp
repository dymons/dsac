#pragma once

#include <examples/dist.consensus.abd/models/request.hpp>
#include <examples/dist.consensus.abd/models/response.hpp>

#include <dsac/container/dynamic_array.hpp>
#include <dsac/functional/expected.hpp>

#include <string>

namespace dsac {

struct peer final {
  std::string host;
  int         port;

  auto execute(std::string const& topic, request const& request) -> dsac::expected<response, std::string>;
};

[[nodiscard]] auto get_peers() -> dynamic_array<peer>;

}  // namespace dsac
