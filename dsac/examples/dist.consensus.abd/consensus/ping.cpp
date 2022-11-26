#include <examples/dist.consensus.abd/consensus/ping.hpp>

namespace dsac {

auto ping::handle([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

} // namespace dsac