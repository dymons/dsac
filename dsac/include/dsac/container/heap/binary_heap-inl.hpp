#ifndef STRUCTURES_BINARY_HEAP_H_
#error This file may only be included from heap.hpp
#endif

#include "../../../../../../../../../../usr/include/c++/11/stdexcept"

namespace dsac::heap {
template <typename T, template <typename> class Compare>
int BinaryHeap<T, Compare>::GetIndexLeftChild(int index) noexcept {
  return 2 * index + 1;
}

template <typename T, template <typename> class Compare>
int BinaryHeap<T, Compare>::GetIndexRightChild(int index) noexcept {
  return 2 * index + 2;
}

template <typename T, template <typename> class Compare>
int BinaryHeap<T, Compare>::GetIndexParent(int index) noexcept {
  return (index - 1) / 2;
}

template <typename T, template <typename> class Compare>
bool BinaryHeap<T, Compare>::ExistLeftChild(int index) noexcept {
  return GetIndexLeftChild(index) < storage_.size();
}

template <typename T, template <typename> class Compare>
bool BinaryHeap<T, Compare>::ExistRightChild(int index) noexcept {
  return GetIndexRightChild(index) < storage_.size();
}

template <typename T, template <typename> class Compare>
bool BinaryHeap<T, Compare>::ExistParent(int index) noexcept {
  return GetIndexParent(index) >= 0;
}

template <typename T, template <typename> class Compare>
T BinaryHeap<T, Compare>::GetLeftChild(int index) noexcept {
  return storage_[GetIndexLeftChild(index)];
}

template <typename T, template <typename> class Compare>
T BinaryHeap<T, Compare>::GetRightChild(int index) noexcept {
  return storage_[GetIndexRightChild(index)];
}

template <typename T, template <typename> class Compare>
T BinaryHeap<T, Compare>::GetParent(int index) noexcept {
  return storage_[GetIndexParent(index)];
}

template <typename T, template <typename> class Compare>
void BinaryHeap<T, Compare>::HeapifyDown() {
  std::size_t index = 0;
  while (ExistLeftChild(index)) {
    std::size_t smaller_child_index = GetIndexLeftChild(index);
    if (ExistRightChild(index) && comp_(GetRightChild(index), GetLeftChild(index))) {
      smaller_child_index = GetIndexRightChild(index);
    }

    if (comp_(storage_[index], storage_[smaller_child_index])) {
      break;
    }

    std::swap(storage_[index], storage_[smaller_child_index]);
    index = smaller_child_index;
  }
}

template <typename T, template <typename> class Compare>
void BinaryHeap<T, Compare>::HeapifyUp() {
  std::size_t last_index = storage_.size() - 1;
  while (ExistParent(last_index) && comp_(storage_[last_index], GetParent(last_index))) {
    std::size_t const parent_index = GetIndexParent(last_index);
    std::swap(storage_[parent_index], storage_[last_index]);
    last_index = parent_index;
  }
}

template <typename T, template <typename> class Compare>
T BinaryHeap<T, Compare>::Peek() const {
  if (storage_.empty()) {
    throw std::out_of_range{""};
  }

  return storage_.front();
}

template <typename T, template <typename> class Compare>
T BinaryHeap<T, Compare>::Pool() {
  const T min_value = Peek();
  storage_.front()  = storage_.back();
  storage_.pop_back();
  HeapifyDown();
  return min_value;
}

template <typename T, template <typename> class Compare>
bool BinaryHeap<T, Compare>::IsEmpty() const noexcept {
  return storage_.empty();
}

template <typename T, template <typename> class Compare>
void BinaryHeap<T, Compare>::Insert(T key) {
  storage_.push_back(key);
  HeapifyUp();
}
}  // namespace dsac::heap