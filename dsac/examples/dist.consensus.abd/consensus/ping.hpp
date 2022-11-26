#pragma once

#include <examples/dist.consensus.abd/consensus/factory.hpp>

namespace dsac {

class ping final : public abd {
  static const inline factory::registractor<ping> kRegistractor;

  auto handle([[maybe_unused]] json request) -> expected<json, std::string> override;

public:
  static std::string get_type_name() {
    return "/ping";
  }
};

}  // namespace dsac