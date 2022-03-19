#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace algo::syncing {
class Semaphore final {
 public:
  explicit Semaphore(std::size_t);

  void Acquire();
  void Release();

 private:
  std::atomic<std::size_t> tokens_;
  std::mutex mutex_;
  std::condition_variable not_zero_;
};
}  // namespace algo::synchronization