#pragma once

#include <memory>

namespace dsac::syncing {
class Barrier final {
 public:
  explicit Barrier(std::size_t size_awaited_threads);
  Barrier(const Barrier&) = delete;
  Barrier(Barrier&&) = delete;
  Barrier& operator=(const Barrier&) = delete;
  Barrier& operator=(Barrier&&) = delete;

  void ArriveAndWait();

 private:
  class Pimpl;
  std::shared_ptr<Pimpl> pimpl_;
};
}  // namespace dsac::synchronization