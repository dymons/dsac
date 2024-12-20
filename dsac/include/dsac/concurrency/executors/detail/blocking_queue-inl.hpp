#ifndef CONCURRENCY_UNBOUNDED_BLOCKING_MPMC_QUEUE_HPP
#error This file may only be included from blocking_queue.hpp
#endif

namespace dsac {

template <typename T>
void unbounded_blocking_mpmc_queue<T>::push(T&& value) {
  {
    auto guard = std::lock_guard{mutex_};
    buffer_.push_back(std::forward<T>(value));
  }
  not_empty_.notify_one();
}

template <typename T>
auto unbounded_blocking_mpmc_queue<T>::pop() -> T {
  auto guard = std::unique_lock{mutex_};
  not_empty_.wait(guard, [this]() { return !buffer_.empty(); });

  return pop_no_lock();
}

template <typename T>
auto unbounded_blocking_mpmc_queue<T>::pop_no_lock() -> T {
  auto data = std::move(buffer_.front());
  buffer_.pop_front();
  return data;
}

}  // namespace dsac
