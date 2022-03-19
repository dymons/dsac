#pragma once

#include <chrono>

// see: https://eli.thegreenplace.net/2018/basics-of-futexes/
namespace algo::syncing {
class Futex final {
 public:
  void Waiting(int* address, int expected_value) const;
  void WakeUp(int* address) const;

  // TODO Разобраться почему под Debug работает, а в Unit Tests постоянно выдается статус
  // EAGAIN вместо ETIMEDOUT
  bool Waiting(int* address, int expected_value, std::chrono::milliseconds timeout) const;
};
}  // namespace algo::synchronization