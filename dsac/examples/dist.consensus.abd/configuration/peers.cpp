#include <examples/dist.consensus.abd/configuration/peers.hpp>
#include <examples/dist.consensus.abd/transport/httplib.hpp>

namespace dsac {

auto peer::execute(std::string topic, request request) -> response {
//  httplib::Client{host, port}.Post(topic, bo);
}

auto get_peers() -> dynamic_array<peer> {
  return {{"0.0.0.0", 8080}, {"0.0.0.0", 8081}, {"0.0.0.0", 8082}};
}

}  // namespace dsac