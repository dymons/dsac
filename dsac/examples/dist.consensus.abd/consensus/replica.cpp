#include <examples/dist.consensus.abd/consensus/replica.hpp>

namespace dsac {

auto replica_set::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

auto replica_get::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

}  // namespace dsac
