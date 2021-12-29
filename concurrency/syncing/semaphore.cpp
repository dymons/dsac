#include <concurrency/syncing/semaphore.hpp>

namespace algo::syncing {
Semaphore::Semaphore(std::size_t consumers) : tokens_(consumers) {
}

void Semaphore::Acquire() {
  std::unique_lock guard(mutex_);
  not_zero_.wait(guard, [this]() {
    return tokens_.load() != 0U;
  });
  --tokens_;
}

void Semaphore::Release() {
  ++tokens_;
  not_zero_.notify_one();
}
}  // namespace algo::syncing