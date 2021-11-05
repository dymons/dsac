#pragma once

#include <vector>
#include <stdexcept>

namespace algo::heap {
template <typename T, typename Compare>
class Heap final {
  std::vector<T> storage_;

  [[gnu::always_inline]] int GetIndexLeftChild(int index) noexcept {
    return 2 * index + 1;
  }

  [[gnu::always_inline]] int GetIndexRightChild(int index) noexcept {
    return 2 * index + 2;
  }

  [[gnu::always_inline]] int GetIndexParent(int index) noexcept {
    return (index - 1) / 2;
  }

  [[gnu::always_inline]] bool ExistLeftChild(int index) noexcept {
    return GetIndexLeftChild(index) < storage_.size();
  }

  [[gnu::always_inline]] bool ExistRightChild(int index) noexcept {
    return GetIndexRightChild(index) < storage_.size();
  }

  [[gnu::always_inline]] bool ExistParent(int index) noexcept {
    return GetIndexParent(index) >= 0;
  }

  [[gnu::always_inline]] T GetLeftChild(std::size_t index) noexcept {
    return storage_[GetIndexLeftChild(index)];
  }

  [[gnu::always_inline]] T GetRightChild(std::size_t index) noexcept {
    return storage_[GetIndexRightChild(index)];
  }

  [[gnu::always_inline]] T GetParent(std::size_t index) noexcept {
    return storage_[GetIndexParent(index)];
  }

  void SwapElementsByIndex(std::size_t left, std::size_t right) {
    std::swap(storage_[left], storage_[right]);
  }

  void HeapifyDown() {
    std::size_t index = 0;
    while (ExistLeftChild(index)) {
      std::size_t smaller_child_index = GetIndexLeftChild(index);
      if (ExistRightChild(index) && Compare{}(GetRightChild(index), GetLeftChild(index))) {
        smaller_child_index = GetIndexRightChild(index);
      }

      if (Compare{}(storage_[index], storage_[smaller_child_index])) {
        break;
      }

      SwapElementsByIndex(index, smaller_child_index);
      index = smaller_child_index;
    }
  }

  void HeapifyUp() {
    std::size_t last_index = storage_.size() - 1;
    while (ExistParent(last_index) && Compare{}(storage_[last_index], GetParent(last_index))) {
      std::size_t const parent_index = GetIndexParent(last_index);
      SwapElementsByIndex(parent_index, last_index);
      last_index = parent_index;
    }
  }

 public:
  T Peek() const {
    if (storage_.empty()) {
      throw std::out_of_range{""};
    }

    return storage_.front();
  }

  T Pool() {
    if (storage_.empty()) {
      throw std::out_of_range{""};
    }

    const T min_value = storage_.front();
    storage_.front() = storage_.back();
    storage_.pop_back();
    HeapifyDown();
    return min_value;
  }

  [[nodiscard]] bool IsEmpty() const noexcept {
    return storage_.empty();
  }

  void Insert(T key) {
    storage_.push_back(key);
    HeapifyUp();
  }
};

template<typename T>
using MinHeap = Heap<T, std::less<T>>;

template<typename T>
using MaxHeap = Heap<T, std::greater<T>>;
}  // namespace algo::heap