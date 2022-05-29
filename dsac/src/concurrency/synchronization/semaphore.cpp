#include <dsac/concurrency/synchronization/semaphore.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace dsac {

class semaphore::pimpl final {
  std::atomic<std::size_t> tokens_;
  std::mutex               mutex_;
  std::condition_variable  not_zero_;

public:
  explicit pimpl(std::size_t consumers);

  void acquire();
  void release();
};

semaphore::pimpl::pimpl(std::size_t consumers)
  : tokens_(consumers) {
}

void semaphore::pimpl::acquire() {
  std::unique_lock guard(mutex_);
  not_zero_.wait(guard, [this]() { return tokens_.load() != 0U; });
  --tokens_;
}

void semaphore::pimpl::release() {
  ++tokens_;
  not_zero_.notify_one();
}

semaphore::semaphore(std::size_t consumers)
  : pimpl_(dsac::make_shared<pimpl>(consumers)) {
}

void semaphore::acquire() {
  pimpl_->acquire();
}

void semaphore::release() {
  pimpl_->release();
}

}  // namespace dsac
