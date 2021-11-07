#pragma once

// see: https://eli.thegreenplace.net/2018/basics-of-futexes/
namespace algo::syncing {
class Futex final {
 public:
  void Waiting(int* address, int expected_value) const;
  void WakeUp(int* address) const;
};
}  // namespace algo::syncing