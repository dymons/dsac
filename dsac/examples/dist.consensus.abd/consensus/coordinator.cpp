#include <examples/dist.consensus.abd/consensus/coordinator.hpp>

namespace dsac {

auto coordinator_set::handle([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

auto coordinator_get::handle([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

} // namespace dsac