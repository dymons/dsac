#pragma once

#include <atomic>

namespace dsac {

class mutex final {
  std::atomic_int state_ = 0;

public:
  void lock();
  void unlock();
};

}  // namespace dsac
