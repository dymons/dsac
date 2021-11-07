#pragma once

#include <atomic>

namespace algo::syncing {
class Mutex final {
  std::atomic_int state_ = 0;

 public:
  void Lock();
  void Unlock();
};
}  // namespace algo::syncing