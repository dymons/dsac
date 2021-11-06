#pragma once

namespace algo::pointers {
template <typename T>
class SharedPtr final {
 private:
  T* data_{nullptr};
  std::size_t* counter_{nullptr};

  void Destroy() {
    if (counter_ != nullptr) {
      --*counter_;
      if (*counter_ == 0) {
        delete data_;
        delete counter_;
      }
    }
  }

 public:
  SharedPtr() noexcept = default;
  explicit SharedPtr(T* data)
      : data_(data), counter_(data != nullptr ? new std::size_t{1u} : nullptr) {
  }

  SharedPtr(const SharedPtr& other) : data_(other.data_), counter_(other.counter_) {
    if (counter_ != nullptr) {
      ++*counter_;
    }
  }

  SharedPtr(SharedPtr&& other) : data_(other.data_), counter_(other.counter_) {
    other.data_ = nullptr;
    other.counter_ = nullptr;
  }

  SharedPtr& operator=(const SharedPtr& other) {
    Destroy();

    data_ = other.data_;
    counter_ = other.counter_;
    if (counter_ != nullptr) {
      ++*counter_;
    }

    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) {
    Destroy();

    data_ = other.data_;
    counter_ = other.counter_;

    other.data_ = nullptr;
    other.counter_ = nullptr;

    return *this;
  }

  ~SharedPtr() {
    Destroy();
  }

  [[nodiscard]] std::size_t UseCount() const noexcept {
    return counter_ == nullptr ? 0u : *counter_;
  }

  [[nodiscard]] T* GetData() const noexcept {
    return data_;
  }

  T& operator*() const {
    return *data_;
  }

  T* operator->() const {
    return data_;
  }
};
}  // namespace algo::pointers