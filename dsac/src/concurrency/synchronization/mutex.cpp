#include <dsac/concurrency/synchronization/mutex.hpp>

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

// 0 means unlocked
// 1 means locked, no waiters
// 2 means locked, there are waiters in lock()

namespace {
int CompareAndSet(std::atomic_int* atom, int expected, int desired) {
  int* ep = &expected;
  std::atomic_compare_exchange_strong(atom, ep, desired);
  return *ep;
}

long int SysCall(int* address, int futex_op, int val) noexcept {
  return syscall(SYS_futex, address, futex_op, val, nullptr, nullptr, 0);
}
}  // namespace

namespace dsac::syncing {
void Mutex::Lock() {
  if (int c = CompareAndSet(&state_, 0, 1); c != 0) {
    do {
      if (c == 2 || CompareAndSet(&state_, 1, 2) != 0) {
        SysCall((int*)&state_, FUTEX_WAIT, 2);
      }
    } while ((c = CompareAndSet(&state_, 0, 2)) != 0);
  }
}

void Mutex::Unlock() {
  if (state_.fetch_sub(1) != 1) {
    state_.store(0);
    SysCall((int*)&state_, FUTEX_WAKE, 1);
  }
}

UniqueLock::UniqueLock(Mutex& mutex) : mutex_(std::addressof(mutex)) {
  Lock();
}

UniqueLock::UniqueLock(UniqueLock&& other) : mutex_(other.mutex_), owned_(other.owned_) {
  other.mutex_ = nullptr;
  other.owned_ = false;
}

UniqueLock& UniqueLock::operator=(UniqueLock&& other) {
  if (owned_) {
    Unlock();
  }

  UniqueLock(std::move(other)).Swap(*this);

  other.mutex_ = nullptr;
  other.owned_ = false;

  return *this;
}

UniqueLock::~UniqueLock() {
  if (owned_) {
    Unlock();
  }
}

void UniqueLock::Swap(UniqueLock& other) {
  std::swap(mutex_, other.mutex_);
  std::swap(owned_, other.owned_);
}

void UniqueLock::Lock() {
  mutex_->Lock();
  owned_ = true;
}

void UniqueLock::Unlock() {
  mutex_->Unlock();
  owned_ = false;
}
}  // namespace dsac::synchronization