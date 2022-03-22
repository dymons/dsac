#pragma once

namespace dsac::pointers {
template <typename T>
class SharedPtr final {
private:
  T*           data_{nullptr};
  std::size_t* counter_{nullptr};

  void Destroy();

public:
  SharedPtr() noexcept = default;
  explicit SharedPtr(T* data);
  SharedPtr(const SharedPtr& other);
  SharedPtr(SharedPtr&& other);
  SharedPtr& operator=(const SharedPtr& other);
  SharedPtr& operator=(SharedPtr&& other);
  ~SharedPtr();

  [[nodiscard]] std::size_t UseCount() const noexcept;
  [[nodiscard]] T*          GetData() const noexcept;

  T& operator*() const;
  T* operator->() const;
};

// TODO Реализовать MakeShared(...)
// TODO Реализовать атомарный счетчик
// TODO Реализовать одно выделение памяти под T и counter в SharedPtr

}  // namespace dsac::pointers

#define CPP_SHARED_PTR_H_
#include "shared_ptr-inl.hpp"
#undef CPP_SHARED_PTR_H_