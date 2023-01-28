#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/replica.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <optional>

namespace dsac::application::command::coordinator {

class read_and_write_register_command_handler final {
  dynamic_array<domain::replica_ref> replicas_;
  domain::policy::quorum_policy_ref  quorum_policy_;

public:
  read_and_write_register_command_handler(
      dynamic_array<domain::replica_ref> replicas,
      domain::policy::quorum_policy_ref  quorum_policy  //
  )
    : replicas_(std::move(replicas))
    , quorum_policy_(std::move(quorum_policy)) {
  }

  [[nodiscard]] auto handle() & -> std::optional<domain::register_value_object>;
};

}  // namespace dsac::application::command::coordinator
