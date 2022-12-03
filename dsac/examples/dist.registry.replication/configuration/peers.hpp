#pragma once

#include <examples/dist.registry.replication/models/request.hpp>
#include <examples/dist.registry.replication/models/response.hpp>

#include <dsac/container/dynamic_array.hpp>
#include <dsac/functional/expected.hpp>

#include <string>

namespace dsac {

class peer final {
  friend auto get_static_configuration() -> dynamic_array<peer>;

  std::string host_;
  int         port_;

  peer(std::string host, int port)
    : host_(std::move(host))
    , port_(port) {
  }

public:
  [[nodiscard]] auto execute(std::string const& topic, request const& request) const
      -> dsac::expected<response, std::string>;
};

[[nodiscard]] auto get_static_configuration() -> dynamic_array<peer>;

}  // namespace dsac
