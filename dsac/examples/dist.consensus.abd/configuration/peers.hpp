#pragma once

#include <examples/dist.consensus.abd/consensus/factory.hpp>

#include <dsac/container/dynamic_array.hpp>

#include <string>

namespace dsac {

struct peer final {
  std::string host;
  int         port;

  auto execute(std::string topic, request request) -> response;
};

[[nodiscard]] auto get_peers() -> dynamic_array<peer>;

}  // namespace dsac
