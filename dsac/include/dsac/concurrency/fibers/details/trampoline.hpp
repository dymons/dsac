#pragma once

namespace dsac {

class trampoline_base {
public:
  virtual ~trampoline_base() = default;

  virtual void run() noexcept = 0;
};

}  // namespace dsac
