#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>

#include <nlohmann/json_fwd.hpp>

#include <cstdint>

namespace dsac::presentation {

struct write_request_dto final {
  std::int32_t                            value{};
  std::size_t                             timestamp{};
  dsac::domain::policy::quorum_policy_ref quorum_policy{nullptr};
};

void from_json(const nlohmann::json& request, write_request_dto& p);

}  // namespace dsac::presentation
