#pragma once

#include <examples/dist.registry.replication//consensus/factory.hpp>

namespace dsac {

class replica_set final : public abd {
  static const inline factory::registractor<replica_set> kRegistractor;

  auto execute(request request) -> expected<response, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/replica/set";
  }
};

class replica_get final : public abd {
  static const inline factory::registractor<replica_get> kRegistractor;

  auto execute(request request) -> expected<response, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/replica/get";
  }
};

class replica_reset final : public abd {
  static const inline factory::registractor<replica_reset> kRegistractor;

  auto execute(request request) -> expected<response, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/replica/reset";
  }
};

}  // namespace dsac
