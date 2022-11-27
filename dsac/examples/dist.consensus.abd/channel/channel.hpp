#pragma once

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class channel_base {
public:
  channel_base()                                   = default;
  channel_base(const channel_base&)                = default;
  channel_base(channel_base&&) noexcept            = default;
  channel_base& operator=(const channel_base&)     = default;
  channel_base& operator=(channel_base&&) noexcept = default;
  virtual ~channel_base()                          = default;

  virtual std::string execute(std::string method, std::string message) = 0;
};

using channel_base_ref = dsac::shared_ptr<channel_base>;

}  // namespace dsac
