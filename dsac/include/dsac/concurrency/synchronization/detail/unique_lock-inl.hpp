#ifndef CONCURRENCY_UNIQUE_LOCK_HPP
#error This file may only be included from unique_lock.hpp
#endif

namespace dsac {

template <typename Mutex>
unique_lock<Mutex>::unique_lock::unique_lock(Mutex& mutex)
  : mutex_(std::addressof(mutex)) {
  lock();
}

template <typename Mutex>
unique_lock<Mutex>::unique_lock(unique_lock&& other) noexcept
  : mutex_(other.mutex_)
  , is_locked_(other.is_locked_) {
  other.mutex_     = nullptr;
  other.is_locked_ = false;
}

template <typename Mutex>
unique_lock<Mutex>& unique_lock<Mutex>::operator=(unique_lock&& other) noexcept {
  if (is_locked_) {
    unlock();
  }

  unique_lock(std::move(other)).swap(*this);

  return *this;
}

template <typename Mutex>
unique_lock<Mutex>::~unique_lock() {
  if (is_locked_) {
    unlock();
  }
}

template <typename Mutex>
void unique_lock<Mutex>::swap(unique_lock& other) {
  std::swap(mutex_, other.mutex_);
  std::swap(is_locked_, other.is_locked_);
}

template <typename Mutex>
void unique_lock<Mutex>::lock() {
  mutex_->lock();
  is_locked_ = true;
}

template <typename Mutex>
void unique_lock<Mutex>::unlock() {
  mutex_->unlock();
  is_locked_ = false;
}

}  // namespace dsac
