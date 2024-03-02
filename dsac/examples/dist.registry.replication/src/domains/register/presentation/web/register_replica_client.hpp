#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/replica.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/futures/future.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <optional>

namespace dsac::presentation::web {

class register_replica_client : public domain::replica_base {
  executor_base_ref executor_;

protected:
  [[nodiscard]] executor_base_ref get_executor() const noexcept {
    return executor_;
  }

public:
  using factory = ::dsac::factory<register_replica_client, executor_base_ref>;

  explicit register_replica_client(executor_base_ref executor)
    : executor_(std::move(executor)) {
  }

  auto async_write(domain::register_value_object const& request) -> future<void*>;

  auto async_read() -> future<domain::register_value_object>;

  virtual auto get_host() -> std::string = 0;

  virtual auto get_port() -> int = 0;
};

#define PEER_NODE_SPAWN_ON_PORT(port)                                    \
  class node##port final : public register_replica_client {              \
    static const inline factory::registractor<node##port> kRegistractor; \
                                                                         \
    auto get_port() -> int final {                                       \
      return port;                                                       \
    }                                                                    \
    auto get_host() -> std::string final {                               \
      return "0.0.0.0";                                                  \
    }                                                                    \
                                                                         \
  public:                                                                \
    using register_replica_client::register_replica_client;              \
    static std::string get_type_name() {                                 \
      return "node" + std::to_string(port);                              \
    }                                                                    \
  };

PEER_NODE_SPAWN_ON_PORT(8080)
PEER_NODE_SPAWN_ON_PORT(8081)
PEER_NODE_SPAWN_ON_PORT(8082)

#undef PEER_NODE_SPAWN_ON_PORT

}  // namespace dsac::presentation::web
