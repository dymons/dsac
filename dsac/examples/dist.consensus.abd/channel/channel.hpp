#pragma once

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

struct request {
  std::string method;
  std::string message;
};

class channel_base {
public:
  channel_base()                                   = default;
  channel_base(const channel_base&)                = default;
  channel_base(channel_base&&) noexcept            = default;
  channel_base& operator=(const channel_base&)     = default;
  channel_base& operator=(channel_base&&) noexcept = default;
  virtual ~channel_base()                          = default;
};

using channel_base_ref = dsac::shared_ptr<channel_base>;

}  // namespace dsac
