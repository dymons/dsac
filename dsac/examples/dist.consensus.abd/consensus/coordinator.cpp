#include <examples/dist.consensus.abd/consensus/coordinator.hpp>

#include <dsac/container/dynamic_array.hpp>
#include <dsac/concurrency/futures/future.hpp>

namespace dsac {

auto coordinator_set::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

auto coordinator_get::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

} // namespace dsac