#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace dsac::syncing {
class Semaphore final {
public:
  explicit Semaphore(std::size_t);

  void Acquire();
  void Release();

private:
  std::atomic<std::size_t> tokens_;
  std::mutex               mutex_;
  std::condition_variable  not_zero_;
};
}  // namespace dsac::syncing