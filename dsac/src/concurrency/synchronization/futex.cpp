#include <dsac/concurrency/synchronization/futex.hpp>

#include <errno.h>
#include <linux/futex.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>

namespace {
enum class Status : unsigned char { Ok, Errno };
[[nodiscard]] Status SysCall(
    int* address, int futex_op, int val, const timespec* timeout = nullptr, int* addr2 = nullptr, int val3 = 0) noexcept
{
  long int status = syscall(SYS_futex, address, futex_op, val, timeout, addr2, val3);
  return status == -1 ? Status::Errno : Status::Ok;
}
}  // namespace

namespace dsac::syncing {
void Futex::Waiting(int* address, int expected_value) const
{
  while (true) {
    Status const status = SysCall(address, FUTEX_WAIT, expected_value);
    if (status == Status::Errno) {
      if (errno != EAGAIN) {
        std::cerr << "Futex error is " << (errno) << std::endl;
        std::terminate();
      }
    }
    else if (*address == expected_value) {
      break;
    }
  }
}

bool Futex::Waiting(int* address, int expected_value, std::chrono::milliseconds timeout) const
{
  const auto     duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout);
  const timespec waiting  = {.tv_sec = 0, .tv_nsec = duration.count()};

  while (true) {
    Status const status = SysCall(address, FUTEX_WAIT, expected_value, &waiting);
    if (status == Status::Errno) {
      if (errno == ETIMEDOUT) {
        return false;
      }
    }
    else if (*address == expected_value) {
      break;
    }
  }

  return true;
}

void Futex::WakeUp(int* address) const
{
  while (true) {
    Status const status = SysCall(address, FUTEX_WAKE, 1);
    if (status == Status::Errno) {
      std::cerr << "Futex error is " << (errno) << std::endl;
      std::terminate();
    }
    else {
      break;
    }
  }
}
}  // namespace dsac::syncing