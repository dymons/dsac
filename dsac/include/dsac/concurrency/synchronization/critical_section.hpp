#pragma once

#include <atomic>

namespace dsac {

class critical_section final {
  std::atomic<bool> locked_{false};

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
