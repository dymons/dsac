#pragma once

#include <atomic>

namespace dsac {

class critical_section final {
  std::atomic<bool> locked_{false};

public:
  void lock();
  void unlock();
};

}  // namespace dsac
