#ifndef CPP_SHARED_PTR_H_
#error This file may only be included from shared_ptr.hpp
#endif

namespace algo::pointers {
template <typename T>
void SharedPtr<T>::Destroy() {
  if (counter_ != nullptr) {
    --*counter_;
    if (*counter_ == 0) {
      delete data_;
      delete counter_;
    }
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(T* data) : data_(data) {
  if (data == nullptr) {
    return;
  }

  try {
    counter_ = new std::size_t{1u};
  } catch (...) {
    delete counter_;
    throw;
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
    : data_(other.data_), counter_(other.counter_) {
  if (counter_ != nullptr) {
    ++*counter_;
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other)
    : data_(other.data_), counter_(other.counter_) {
  other.data_ = nullptr;
  other.counter_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
  Destroy();

  data_ = other.data_;
  counter_ = other.counter_;
  if (counter_ != nullptr) {
    ++*counter_;
  }

  return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) {
  Destroy();

  data_ = other.data_;
  counter_ = other.counter_;

  other.data_ = nullptr;
  other.counter_ = nullptr;

  return *this;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
  Destroy();
}

template <typename T>
std::size_t SharedPtr<T>::UseCount() const noexcept {
  return counter_ == nullptr ? 0u : *counter_;
}

template <typename T>
T* SharedPtr<T>::GetData() const noexcept {
  return data_;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
  return *data_;
}

template <typename T>
T* SharedPtr<T>::operator->() const {
  return data_;
}
}  // namespace algo::pointers