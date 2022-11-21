#pragma once

#include <dsac/memory/shared_ptr.hpp>

#include <examples/dist.consensus.abd/transport/transport.hpp>

namespace dsac {

class transport_http final : public transport_base {
public:
  transport_http();

  void serve(int port) override;

private:
  class transport_http_pimpl;
  shared_ptr<transport_http_pimpl> pimpl;
};

}  // namespace dsac
