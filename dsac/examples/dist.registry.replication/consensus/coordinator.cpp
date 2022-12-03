#include <examples/dist.registry.replication//consensus/coordinator.hpp>

namespace dsac {

auto coordinator_set::execute(request request) -> expected<response, std::string> {
  return {};
}

auto coordinator_get::execute(request request) -> expected<response, std::string> {
  return {};
}

} // namespace dsac