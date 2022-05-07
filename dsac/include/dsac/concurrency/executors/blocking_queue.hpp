#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

// Unbounded Blocking Multi-Producer/Multi-Consumer Queue

namespace dsac {
template <typename T>
class UnboundedBlockingMPMCQueue final {
public:
  void Push(T value) {
    std::lock_guard guard(mutex_);
    buffer_.push_back(std::move(value));
    not_empty_.notify_one();
  }

  T Pop() {
    std::unique_lock guard(mutex_);
    not_empty_.wait(guard, [this]() { return !buffer_.empty(); });

    return Pop_NoLock();
  }

private:
  T Pop_NoLock() {
    T data = std::move(buffer_.front());
    buffer_.pop_front();
    return data;
  }

  std::mutex              mutex_;
  std::deque<T>           buffer_;  // Guarded by mutex_
  std::condition_variable not_empty_;
};
}  // namespace dsac