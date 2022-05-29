#pragma once

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class semaphore final {
public:
  explicit semaphore(std::size_t );

  void acquire();
  void release();

private:
  class pimpl;
  dsac::shared_ptr<pimpl> pimpl_;
};

}  // namespace dsac
