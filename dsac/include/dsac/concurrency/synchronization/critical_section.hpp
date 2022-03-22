#pragma once

#include <atomic>

namespace dsac::syncing {
// an implementation of test-and-set spinlock
// see: https://en.wikipedia.org/wiki/Test-and-set
class CriticalSection final {
  std::atomic<bool> locked_{false};

public:
  void Lock();
  void Unlock();
};
}  // namespace dsac::syncing