#pragma once

#include <atomic>

namespace dsac {

class mutex final {
  std::atomic_int state_ = 0;

public:
  /*!
    \brief
        Locks the mutex, blocks if the mutex is not available
  */
  void lock();

  /*!
    \brief
        Unlocks the mutex
  */
  void unlock();
};

}  // namespace dsac
