#include <dsac/concurrency/synchronization/barrier.hpp>

#include <mutex>
#include <condition_variable>

namespace dsac::syncing {
class Barrier::Pimpl final {
  std::mutex mutex_;
  std::condition_variable cv_;

  const std::size_t size_awaited_threads_;
  std::size_t current_awaited_threads_;

  enum class State : unsigned char { Blocking, Unblocking };
  State state_;

 public:
  explicit Pimpl(std::size_t size_awaited_threads)
      : size_awaited_threads_(size_awaited_threads),
        current_awaited_threads_(size_awaited_threads),
        state_(State::Blocking) {
  }

  Pimpl(const Pimpl&) = delete;
  Pimpl(Pimpl&&) = delete;
  Pimpl& operator=(const Pimpl&) = delete;
  Pimpl& operator=(Pimpl&&) = delete;

  void ArriveAndWait() {
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
};

Barrier::Barrier(std::size_t size_awaited_threads)
    : pimpl_(std::make_shared<Pimpl>(size_awaited_threads)) {
}

void Barrier::ArriveAndWait() {
  pimpl_->ArriveAndWait();
}
}  // namespace dsac::syncing