#include <concurrency/syncing/barrier.hpp>

namespace algo::syncing {
Barrier::Barrier(std::size_t size_awaited_threads)
    : size_awaited_threads_(size_awaited_threads) {
}

void Barrier::ArriveAndWait() {
  std::unique_lock<std::mutex> lock{mutex_};
  if (--size_awaited_threads_ == 0u) {
    cv_.notify_all();
  } else {
    cv_.wait(lock, [this]() {
      return size_awaited_threads_ == 0u;
    });
  }
}
}  // namespace algo::syncing