#pragma once

#include <examples/dist.registry.replication//consensus/factory.hpp>

namespace dsac {

class coordinator_set final : public abd {
  static const inline factory::registractor<coordinator_set> kRegistractor;

  auto execute(request request) -> expected<response, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/register/set";
  }
};

class coordinator_get final : public abd {
  static const inline factory::registractor<coordinator_get> kRegistractor;

  auto execute(request request) -> expected<response, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/register/get";
  }
};

}  // namespace dsac