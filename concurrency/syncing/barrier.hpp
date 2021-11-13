#pragma once

#include <mutex>
#include <condition_variable>

namespace algo::syncing {
class Barrier final {
  std::mutex mutex_;
  std::condition_variable cv_;
  std::size_t size_awaited_threads_;

 public:
  explicit Barrier(std::size_t size_awaited_threads);
  Barrier(const Barrier&) = delete;
  Barrier(Barrier&&) = delete;
  Barrier& operator=(const Barrier&) = delete;
  Barrier& operator=(Barrier&&) = delete;

  void ArriveAndWait();
};
}  // namespace algo::syncing