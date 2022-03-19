#pragma once

#include <vector>

namespace algo::sort {
namespace detail {
class SortingTree {
 public:
  template <typename T>
  static void Heapify(std::vector<T>& arr) {
    for (int i = arr.size() / 2 - 1; i >= 0; i--) {
      SortingTree::Heapify(arr, arr.size(), i);
    }
  }

  template <typename T>
  static void Heapify(std::vector<T>& arr, const int size, const int root) {
    int largest = root;
    const int left = 2 * root + 1;
    const int right = 2 * root + 2;

    if ((left < size) && (arr[left] > arr[largest])) {
      largest = left;
    }

    if ((right < size) && (arr[right] > arr[largest])) {
      largest = right;
    }

    if (largest != root) {
      std::swap(arr[root], arr[largest]);
      SortingTree::Heapify(arr, size, largest);
    }
  }

  template <typename T>
  static void Sort(std::vector<T>& arr) {
    SortingTree::Heapify(arr);
    for (int i = arr.size() - 1; i >= 0; i--) {
      std::swap(arr[0], arr[i]);
      SortingTree::Heapify(arr, i, 0);
    }
  }
};
}  // namespace detail

template <typename T>
void HeapSort(std::vector<T>& arr) {
  detail::SortingTree::Sort(arr);
}
}  // namespace algo::sort