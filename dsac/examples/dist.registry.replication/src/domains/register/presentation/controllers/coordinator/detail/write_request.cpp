#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/detail/write_request.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/exception.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation {

using dsac::domain::policy::quorum_policy_base;

void from_json(const nlohmann::json& request, write_request_dto& p) {
  if (!request.contains("value") || !request["value"].is_number_integer()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'value' is missing"};
  }
  if (!request.contains("timestamp") || !request["timestamp"].is_number_unsigned()) [[unlikely]] {
    throw dsac::presentation::invalid_argument{"Input data is incorrect or the required field 'timestamp' is missing"};
  }

  if (request.contains("quorum_policy_base")) {
    if (!request["quorum_policy_base"].is_string()) [[unlikely]] {
      throw dsac::presentation::invalid_argument{
          "Input data is incorrect or the required field 'quorum_policy_base' is missing"};
    }
    p.quorum_policy =
        dsac::shared_ptr{quorum_policy_base::factory::construct(request["quorum_policy_base"].get<std::string>()).release()};
  }

  p.value     = request["value"].get<std::int32_t>();
  p.timestamp = request["timestamp"].get<std::size_t>();
}

}  // namespace dsac::presentation