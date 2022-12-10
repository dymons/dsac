#include <examples/dist.registry.replication/src/domains/register/presentation/controllers/coordinator/read_register_handler.hpp>

#include <nlohmann/json.hpp>

namespace dsac::presentation::coordinator {

auto read_register_handler::handle(nlohmann::json const& request) -> nlohmann::json {
  return {};
}

}  // namespace dsac::presentation::coordinator
