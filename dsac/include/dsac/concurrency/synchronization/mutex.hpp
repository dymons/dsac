#pragma once

#include <atomic>

namespace dsac::syncing {
class Mutex final {
  std::atomic_int state_ = 0;

public:
  void Lock();
  void Unlock();
};

class UniqueLock final {
public:
  explicit UniqueLock(Mutex& mutex);
  ~UniqueLock();

  UniqueLock(const UniqueLock&) = delete;
  UniqueLock& operator=(const UniqueLock&) = delete;
  UniqueLock(UniqueLock&&);
  UniqueLock& operator=(UniqueLock&&);

  void Lock();
  void Unlock();

private:
  void Swap(UniqueLock& other);

private:
  Mutex* mutex_ = nullptr;
  bool   owned_ = false;
};
}  // namespace dsac::syncing