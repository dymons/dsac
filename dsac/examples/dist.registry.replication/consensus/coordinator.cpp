#include <examples/dist.registry.replication/configuration/peers.hpp>
#include <examples/dist.registry.replication/consensus/coordinator.hpp>
#include <examples/dist.registry.replication/consensus/replica.hpp>

#include <dsac/container/dynamic_array.hpp>

namespace dsac {

auto coordinator_set::execute(request request) -> expected<response, std::string> {
  if (!request.value.has_value() || !request.timestamp.has_value()) {
    return dsac::make_unexpected("Input data is incorrect for consensus algorithm");
  }

  for (peer const& peer : get_static_configuration()) {
    dsac::request peer_request{.value = request.value.value(), .timestamp = request.timestamp.value()};
    [[maybe_unused]] expected<response, std::string> response =
        peer.execute(replica_set::get_type_name(), peer_request);
  }

  return response{};
}

auto coordinator_get::execute([[maybe_unused]] request request) -> expected<response, std::string> {
  dynamic_array<response> responses;
  for (peer const& peer : get_static_configuration()) {
    expected<response, std::string> response = peer.execute(replica_get::get_type_name(), dsac::request{});
    if (response.has_value()) {
      responses.push_back(response.value());
    }
  }

  dynamic_array<response>::iterator const register_value =
      std::max_element(responses.begin(), responses.end(), [](response const& lhs, response const& rhs) {
        return lhs.timestamp > rhs.timestamp;
      });

  return response{
      .value     = register_value->value,
      .timestamp = register_value->timestamp,
  };
}

}  // namespace dsac