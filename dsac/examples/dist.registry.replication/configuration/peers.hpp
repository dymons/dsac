#pragma once

#include <examples/dist.registry.replication/models/request.hpp>
#include <examples/dist.registry.replication/models/response.hpp>

#include <dsac/container/dynamic_array.hpp>
#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <string>

namespace dsac {

class peer {
public:
  using factory = ::dsac::factory<peer>;

  peer()                           = default;
  peer(const peer&)                = default;
  peer(peer&&) noexcept            = default;
  peer& operator=(const peer&)     = default;
  peer& operator=(peer&&) noexcept = default;
  virtual ~peer()                  = default;

  auto         execute(std::string topic, request request) -> expected<response, std::string>;
  virtual auto get_host() -> std::string = 0;
  virtual auto get_port() -> int         = 0;
};

#define PEER_NODE_SPAWN_ON_PORT(port)                                                                              \
  class node##port final : public peer {                                                                           \
    static const inline factory::registractor<node##port> kRegistractor;                                           \
    auto                                                  get_port() -> int override { return (port); }            \
    auto                                                  get_host() -> std::string override { return "0.0.0.0"; } \
                                                                                                                   \
  public:                                                                                                          \
    static std::string get_type_name() { return "node" + std::to_string(port); }                                                 \
  };

PEER_NODE_SPAWN_ON_PORT(8080)
PEER_NODE_SPAWN_ON_PORT(8081)
PEER_NODE_SPAWN_ON_PORT(8082)

#undef PEER_NODE_SPAWN_ON_PORT

}  // namespace dsac
