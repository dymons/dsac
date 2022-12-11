#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/pattern/registrator/registrator.hpp>

#include <optional>

namespace dsac::presentation::web {

class register_replica_client {
public:
  using factory = ::dsac::factory<register_replica_client>;

  register_replica_client()                                              = default;
  register_replica_client(const register_replica_client&)                = default;
  register_replica_client(register_replica_client&&) noexcept            = default;
  register_replica_client& operator=(const register_replica_client&)     = default;
  register_replica_client& operator=(register_replica_client&&) noexcept = default;
  virtual ~register_replica_client()                                     = default;

  auto         write(domain::register_dto const& request) -> void;
  auto         read() -> std::optional<domain::register_dto>;
  virtual auto get_host() -> std::string = 0;
  virtual auto get_port() -> int         = 0;
};

#define PEER_NODE_SPAWN_ON_PORT(port)                                                                              \
  class node##port final : public register_replica_client {                                                        \
    static const inline factory::registractor<node##port> kRegistractor;                                           \
    auto                                                  get_port() -> int override { return (port); }            \
    auto                                                  get_host() -> std::string override { return "0.0.0.0"; } \
                                                                                                                   \
  public:                                                                                                          \
    using register_replica_client::register_replica_client;                                                        \
    static std::string get_type_name() { return "node" + std::to_string(port); }                                   \
  };

PEER_NODE_SPAWN_ON_PORT(8080)
PEER_NODE_SPAWN_ON_PORT(8081)
PEER_NODE_SPAWN_ON_PORT(8082)

#undef PEER_NODE_SPAWN_ON_PORT

}  // namespace dsac::presentation::web