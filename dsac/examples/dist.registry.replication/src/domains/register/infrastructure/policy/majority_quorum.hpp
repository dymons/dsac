#pragma once

#include "examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp"

namespace dsac::infrastructure::quorum {

class majority_quorum_policy final : public domain::policy::quorum_policy_base {
  static const inline factory::registractor<majority_quorum_policy> kRegistractor;

public:
  [[nodiscard]] auto quorum(std::size_t n) const -> std::size_t override {
    return n / 2 + 1;
  }
  static auto get_type_name() -> std::string {
    return "majority";
  }
};

}  // namespace dsac::infrastructure::quorum