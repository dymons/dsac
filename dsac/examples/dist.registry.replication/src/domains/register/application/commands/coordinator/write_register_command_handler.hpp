#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/executors/executor.hpp>

#include <cstdint>

namespace dsac::application::command::coordinator {

struct write_register_command final {
  domain::register_value_object new_register_value;
};

class write_register_command_handler final {
  const executor_base_ref                 executor_;
  const domain::policy::quorum_policy_ref quorum_policy_;

public:
  explicit write_register_command_handler(executor_base_ref executor, domain::policy::quorum_policy_ref quorum_policy)
    : executor_(std::move(executor))
    , quorum_policy_(std::move(quorum_policy)) {
  }

  auto handle(write_register_command const& command) & -> void;
};

}  // namespace dsac::application::command::coordinator
