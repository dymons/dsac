#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

namespace dsac {

template <typename T>
class unbounded_blocking_mpmc_queue final {
public:
  // Modifiers

  void push(T&& value);

  [[nodiscard]] auto pop() -> T;

private:
  [[nodiscard]] auto pop_no_lock() -> T;

  /// Provides exclusive access of modifying functions to the buffer_
  std::mutex    mutex_;
  std::deque<T> buffer_;  // Guarded by mutex_

  /// Synchronization primitive for implementing blocking waiting in case of an empty queue
  std::condition_variable not_empty_;
};

}  // namespace dsac

#define CONCURRENCY_UNBOUNDED_BLOCKING_MPMC_QUEUE_HPP
#include <dsac/concurrency/executors/detail/blocking_queue-inl.hpp>
#undef CONCURRENCY_UNBOUNDED_BLOCKING_MPMC_QUEUE_HPP
