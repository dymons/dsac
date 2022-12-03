#pragma once

#include <examples/dist.registry.replication//models/request.hpp>
#include <examples/dist.registry.replication//models/response.hpp>

#include <dsac/container/dynamic_array.hpp>
#include <dsac/functional/expected.hpp>

#include <string>

namespace dsac {

struct peer final {
  std::string host;
  int         port;

  auto execute(std::string const& topic, request const& request) const -> dsac::expected<response, std::string>;
};

[[nodiscard]] auto get_peers() -> dynamic_array<peer>;

}  // namespace dsac
