#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

// Unbounded Blocking Multi-Producer/Multi-Consumer Queue

namespace dsac {
template <typename T>
class unbounded_blocking_mpmc_queue final {
public:
  void push(T&& value) {
    std::lock_guard guard(mutex_);
    buffer_.push_back(std::forward<T>(value));
    not_empty_.notify_one();
  }

  T pop() {
    std::unique_lock guard(mutex_);
    not_empty_.wait(guard, [this]() { return !buffer_.empty(); });

    return pop_no_lock();
  }

private:
  T pop_no_lock() {
    T data = std::move(buffer_.front());
    buffer_.pop_front();
    return data;
  }

  std::mutex              mutex_;
  std::deque<T>           buffer_;  // Guarded by mutex_
  std::condition_variable not_empty_;
};
}  // namespace dsac