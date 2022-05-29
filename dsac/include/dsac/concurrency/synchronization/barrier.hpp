#pragma once

#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class barrier final {
public:
  // Constructors

  /*!
    \brief
        User constructor, constructs an barrier which provides a thread-coordination mechanism that allows at most an
        expected number of threads to block.
  */
  explicit barrier(std::size_t size_awaited_threads);

  /*!
    \brief
        Copy constructor.
  */
  barrier(const barrier&) = delete;

  /*!
    \brief
        Move constructor.
  */
  barrier(barrier&&) = delete;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~barrier() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  barrier& operator=(const barrier&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  barrier& operator=(barrier&&) = delete;

  // Modifiers

  /*!
    \brief
        Arrives at barrier and decrements the expected count by one, then blocks until current phase completes .

    \ingroup
          DsacConcurrency
  */
  void arrive_and_wait();

private:
  /// Pointer to the implementation barrier through an idiom Pimpl
  class pimpl;
  dsac::shared_ptr<pimpl> pimpl_;
};

}  // namespace dsac
