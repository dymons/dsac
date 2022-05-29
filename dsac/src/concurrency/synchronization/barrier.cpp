#include <dsac/concurrency/synchronization/barrier.hpp>

#include <condition_variable>
#include <mutex>

namespace dsac {

class barrier::pimpl final {
  std::mutex              mutex_;
  std::condition_variable cv_;

  const std::size_t size_awaited_threads_;
  std::size_t       current_awaited_threads_;

  enum class state : unsigned char { blocking, unblocking };
  state state_{state::blocking};

public:
  explicit pimpl(std::size_t size_awaited_threads)
    : size_awaited_threads_(size_awaited_threads)
    , current_awaited_threads_(size_awaited_threads) {
  }

  pimpl(const pimpl&)            = delete;
  pimpl(pimpl&&)                 = delete;
  pimpl& operator=(const pimpl&) = delete;
  pimpl& operator=(pimpl&&)      = delete;
  ~pimpl()                       = default;

  void arrive_and_wait() {
    std::unique_lock<std::mutex> lock{mutex_};
    if (state_ == state::blocking) {
      if (--current_awaited_threads_ == 0u) {
        state_ = state::unblocking;
        cv_.notify_all();
      } else {
        cv_.wait(lock, [this]() { return state_ == state::unblocking; });
      }
    } else {
      if (++current_awaited_threads_ == size_awaited_threads_) {
        state_ = state::blocking;
        cv_.notify_all();
      } else {
        cv_.wait(lock, [this] { return state_ == state::blocking; });
      }
    }
  }
};

barrier::barrier(std::size_t size_awaited_threads)
  : pimpl_(dsac::make_shared<pimpl>(size_awaited_threads)) {
}

void barrier::arrive_and_wait() {
  pimpl_->arrive_and_wait();
}

}  // namespace dsac
