#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/executors/executor.hpp>

#include <cstdint>
#include <optional>

namespace dsac::application::command::coordinator {

class read_and_write_register_command_handler final {
  const executor_base_ref                 executor_;
  const domain::policy::quorum_policy_ref quorum_policy_;

public:
  explicit read_and_write_register_command_handler(executor_base_ref executor, domain::policy::quorum_policy_ref quorum_policy)
    : executor_(std::move(executor))
    , quorum_policy_(std::move(quorum_policy)) {
  }

  [[nodiscard]] auto handle() const -> std::optional<domain::register_dto>;
};

}  // namespace dsac::application::command::coordinator
