#include <concurrency/syncing/barrier.hpp>

namespace algo::syncing {
Barrier::Barrier(std::size_t size_awaited_threads)
    : size_awaited_threads_(size_awaited_threads),
      current_awaited_threads_(size_awaited_threads),
      state_(State::Blocking) {
}

void Barrier::ArriveAndWait() {
  std::unique_lock<std::mutex> lock{mutex_};
  if (state_ == State::Blocking) {
    if (--current_awaited_threads_ == 0u) {
      state_ = State::Unblocking;
      cv_.notify_all();
    } else {
      cv_.wait(lock, [this]() {
        return state_ == State::Unblocking;
      });
    }
  } else {
    if (++current_awaited_threads_ == size_awaited_threads_) {
      state_ = State::Blocking;
      cv_.notify_all();
    } else {
      cv_.wait(lock, [this] {
        return state_ == State::Blocking;
      });
    }
  }
}
}  // namespace algo::syncing