#pragma once

#include <string>

namespace dsac {

class transport_base {
public:
  transport_base()                                     = default;
  transport_base(const transport_base&)                = default;
  transport_base(transport_base&&) noexcept            = default;
  transport_base& operator=(const transport_base&)     = default;
  transport_base& operator=(transport_base&&) noexcept = default;
  virtual ~transport_base()                            = default;

  virtual void serve(int port) = 0;
};

}  // namespace dsac
