#pragma once

#include <dsac/memory/shared_ptr.hpp>

#include <cstdint>

namespace dsac::domain::policy {

class quorum_policy {
public:
  quorum_policy()                                    = default;
  quorum_policy(const quorum_policy&)                = default;
  quorum_policy(quorum_policy&&) noexcept            = default;
  quorum_policy& operator=(const quorum_policy&)     = default;
  quorum_policy& operator=(quorum_policy&&) noexcept = default;
  virtual ~quorum_policy()                           = default;

  [[nodiscard]] virtual auto quorum(std::size_t n) const -> std::size_t = 0;
};

using quorum_policy_ref = shared_ptr<quorum_policy>;

}  // namespace dsac::domain::policy