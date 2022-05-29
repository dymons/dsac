#pragma once

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class semaphore final {
public:
  /*!
    \brief
        User constructor, constructs a semaphore with fixed allowed consumers.
  */
  explicit semaphore(std::size_t consumers);

  /*!
    \brief
        Decrements the internal counter or blocks until it can
  */
  void acquire();

  /*!
    \brief
        increments the internal counter and unblocks acquirers
  */
  void release();

private:
  /// Pointer to the implementation semaphore through an idiom Pimpl
  class pimpl;
  dsac::shared_ptr<pimpl> pimpl_;
};

}  // namespace dsac
