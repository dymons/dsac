#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/replica.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <cstdint>

namespace dsac::application::command::coordinator {

struct write_register_command final {
  domain::register_value_object new_register_value;
};

class write_register_command_handler final {
  dynamic_array<domain::replica_ref> replicas_;
  domain::policy::quorum_policy_ref  quorum_policy_;

public:
  explicit write_register_command_handler(
      dynamic_array<domain::replica_ref> replicas,
      domain::policy::quorum_policy_ref  quorum_policy  //
  )
    : replicas_(std::move(replicas))
    , quorum_policy_(std::move(quorum_policy)) {
  }

  auto handle(write_register_command const& command) & -> void;
};

}  // namespace dsac::application::command::coordinator
