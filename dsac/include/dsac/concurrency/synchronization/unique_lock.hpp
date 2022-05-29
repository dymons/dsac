#pragma once

namespace dsac {

template <typename Mutex>
class unique_lock final {
public:
  // Constructors

  /*!
    \brief
        User constructor, constructs an unique_lock with predefined mutex for locking.
  */
  explicit unique_lock(Mutex& mutex);

  /*!
    \brief
        Copy constructor.
  */
  unique_lock(const unique_lock&) = delete;

  /*!
    \brief
        Move constructor.
  */
  unique_lock(unique_lock&& other) noexcept;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~unique_lock();

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  unique_lock& operator=(const unique_lock&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  unique_lock& operator=(unique_lock&& other) noexcept;

  // Modifiers

  /*!
    \brief
        Locks (i.e., takes ownership of) the associated mutex
  */
  void lock();

  /*!
    \brief
        Unlocks (i.e., releases ownership of) the associated mutex
  */
  void unlock();

private:
  /*!
    \brief
        Swaps state with another std::unique_lock
  */
  void swap(unique_lock& other);

  /// Pointer to the Mutex which is ownership
  Mutex* mutex_ = nullptr;

  /// Was there a mutex of unlocks
  bool is_locked_ = false;
};

}  // namespace dsac

#define CONCURRENCY_UNIQUE_LOCK_HPP
#include <dsac/concurrency/synchronization/detail/unique_lock-inl.hpp>
#undef CONCURRENCY_UNIQUE_LOCK_HPP
