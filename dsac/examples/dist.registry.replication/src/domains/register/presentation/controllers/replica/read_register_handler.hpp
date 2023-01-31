#include <examples/dist.registry.replication/src/domains/register/presentation/controller_factory.hpp>

namespace dsac::presentation::replica {

class read_register_handler final : public controller {
  static const inline factory::registractor<read_register_handler> kRegistractor;

  auto handle(nlohmann::json const& request) -> nlohmann::json final;

public:
  using controller::controller;

  static auto get_type_name() -> std::string {
    return "/v1/replica/read";
  }
};

}  // namespace dsac::presentation::replica
