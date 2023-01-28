#pragma once

#include <dsac/memory/shared_ptr.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <cstdint>

namespace dsac::domain::policy {

class quorum_policy_base {
public:
  using factory = ::dsac::factory<quorum_policy_base>;

  quorum_policy_base()                                         = default;
  quorum_policy_base(const quorum_policy_base&)                = delete;
  quorum_policy_base(quorum_policy_base&&) noexcept            = delete;
  quorum_policy_base& operator=(const quorum_policy_base&)     = delete;
  quorum_policy_base& operator=(quorum_policy_base&&) noexcept = delete;
  virtual ~quorum_policy_base()                                = default;

  [[nodiscard]] virtual auto quorum(std::size_t n) const -> std::size_t = 0;
};

using quorum_policy_ref = shared_ptr<quorum_policy_base>;

}  // namespace dsac::domain::policy