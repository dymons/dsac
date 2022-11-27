#pragma once

#include <examples/dist.consensus.abd/transport/transport.hpp>

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class transport_http final : public transport_base {
public:
  transport_http();

  void serve(int port) override;
};

}  // namespace dsac
