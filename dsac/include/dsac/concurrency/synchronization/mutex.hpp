#pragma once

#include <atomic>

namespace dsac {
class mutex final {
  std::atomic_int state_ = 0;

public:
  void lock();
  void unlock();
};

class unique_lock final {
public:
  explicit unique_lock(mutex& mutex);
  ~unique_lock();

  unique_lock(const unique_lock&)            = delete;
  unique_lock& operator=(const unique_lock&) = delete;
  unique_lock(unique_lock&&) noexcept ;
  unique_lock& operator=(unique_lock&&) noexcept ;

  void lock();
  void unlock();

private:
  void swap(unique_lock& other);

  mutex* mutex_ = nullptr;
  bool   owned_ = false;
};
}  // namespace dsac