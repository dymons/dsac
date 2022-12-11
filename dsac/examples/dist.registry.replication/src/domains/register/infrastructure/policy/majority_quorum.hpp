#pragma once

#include "examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp"

namespace dsac::infrastructure::quorum {

class majority_quorum_policy final : public domain::policy::quorum_policy {
public:
  [[nodiscard]] auto quorum(std::size_t n) const -> std::size_t override {
    return n / 2 + 1;
  }
};

}  // namespace dsac::infrastructure::quorum