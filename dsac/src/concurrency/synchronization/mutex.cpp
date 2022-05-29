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

namespace dsac {
void mutex::lock() {
  if (int c = CompareAndSet(&state_, 0, 1); c != 0) {
    do {
      if (c == 2 || CompareAndSet(&state_, 1, 2) != 0) {
        SysCall((int*)&state_, FUTEX_WAIT, 2);
      }
    } while ((c = CompareAndSet(&state_, 0, 2)) != 0);
  }
}

void mutex::unlock() {
  if (state_.fetch_sub(1) != 1) {
    state_.store(0);
    SysCall((int*)&state_, FUTEX_WAKE, 1);
  }
}

}  // namespace dsac