#pragma once

#include <examples/dist.consensus.abd/consensus/factory.hpp>

namespace dsac {

class coordinator_set final : public abd {
  static const inline factory::registractor<coordinator_set> kRegistractor;

  auto handle([[maybe_unused]] json request) -> expected<json, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/coordinator/set";
  }
};

class coordinator_get final : public abd {
  static const inline factory::registractor<coordinator_get> kRegistractor;

  auto handle([[maybe_unused]] json request) -> expected<json, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/coordinator/get";
  }
};

}  // namespace dsac