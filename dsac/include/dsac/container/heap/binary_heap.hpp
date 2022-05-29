#pragma once

#include "../../../../../../../../../../usr/include/c++/11/vector"

namespace dsac::heap {
template <typename T, template <typename> class Compare>
class BinaryHeap final {
  [[gnu::always_inline]] inline int  GetIndexLeftChild(int index) noexcept;
  [[gnu::always_inline]] inline int  GetIndexRightChild(int index) noexcept;
  [[gnu::always_inline]] inline int  GetIndexParent(int index) noexcept;
  [[gnu::always_inline]] inline bool ExistLeftChild(int index) noexcept;
  [[gnu::always_inline]] inline bool ExistRightChild(int index) noexcept;
  [[gnu::always_inline]] inline bool ExistParent(int index) noexcept;
  [[gnu::always_inline]] inline T    GetLeftChild(int index) noexcept;
  [[gnu::always_inline]] inline T    GetRightChild(int index) noexcept;
  [[gnu::always_inline]] inline T    GetParent(int index) noexcept;

  void HeapifyDown();
  void HeapifyUp();

  std::vector<T> storage_;
  Compare<T>     comp_;

public:
  T                  Peek() const;
  [[nodiscard]] bool IsEmpty() const noexcept;

  T    Pool();
  void Insert(T key);
};

template <typename T>
using MinBinaryHeap = BinaryHeap<T, std::less>;

template <typename T>
using MaxBinaryHeap = BinaryHeap<T, std::greater>;
}  // namespace dsac::heap

#define STRUCTURES_BINARY_HEAP_H_
#include "binary_heap-inl.hpp"
#undef STRUCTURES_BINARY_HEAP_H_