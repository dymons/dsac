#include <examples/dist.registry.replication//consensus/coordinator.hpp>

namespace dsac {

auto coordinator_set::execute(request request) -> expected<response, std::string> {
  if (!request.value.has_value()) {
    return dsac::make_unexpected("Input data is incorrect for consensus algorithm");
  }



  return {};
}

auto coordinator_get::execute(request request) -> expected<response, std::string> {
  return {};
}

} // namespace dsac