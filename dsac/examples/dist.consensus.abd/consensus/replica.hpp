#pragma once

#include <examples/dist.consensus.abd/consensus/factory.hpp>

namespace dsac {

class replica_set final : public abd {
  static const inline factory::registractor<replica_set> kRegistractor;

  auto execute([[maybe_unused]] json request) -> expected<json, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/replica/set";
  }
};

class replica_get final : public abd {
  static const inline factory::registractor<replica_get> kRegistractor;

  auto execute([[maybe_unused]] json request) -> expected<json, std::string> override;

public:
  static std::string get_type_name() {
    return "/v1/replica/get";
  }
};

}  // namespace dsac
