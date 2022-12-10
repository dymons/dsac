#include <examples/dist.registry.replication/src/domains/register/presentation/controller_factory.hpp>

namespace dsac::presentation::coordinator {

class write_register_handler final : public controller {
  static const inline factory::registractor<write_register_handler> kRegistractor;

  auto handle(nlohmann::json const& request) -> nlohmann::json override;

public:
  static auto get_type_name() -> std::string {
    return "/v1/coordinator/write";
  }
};

}  // namespace dsac::presentation::coordinator
