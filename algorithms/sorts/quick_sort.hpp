#pragma once

#include <vector>

namespace algo::sort::v1 {
namespace detail {
template <typename T>
void QuickSortImpl(std::vector<T>& arr, const int low, const int high) {
  if (low < high) {
    int left = low;
    int right = high;
    const int pivot = arr[(low + high) / 2];

    while (left <= right) {
      while (arr[left] < pivot) {
        ++left;
      }
      while (arr[right] > pivot) {
        --right;
      }

      if (left <= right) {
        std::swap(arr[left], arr[right]);
        ++left;
        --right;
      }
    }

    QuickSortImpl(arr, low, right);
    QuickSortImpl(arr, left, high);
  }
}
}  // namespace detail

template <typename T>
void QuickSort(std::vector<T>& arr) {
  detail::QuickSortImpl(arr, 0, (int)arr.size() - 1);
}
}  // namespace algo::sort::v1

namespace algo::sort::v2 {
namespace detail {
template <typename T>
void QuickSortImpl(std::vector<T>& arr, const int low, const int high) {
  if (low < high) {
    int left = low;
    const int pivot = arr[high];
    for (int i = low; i < high; ++i) {
      if (arr[i] < pivot) {
        std::swap(arr[left], arr[i]);
        ++left;
      }
    }
    std::swap(arr[left], arr[high]);

    QuickSortImpl(arr, low, left - 1);
    QuickSortImpl(arr, left + 1, high);
  }
}
}  // namespace detail

template <typename T>
void QuickSort(std::vector<T>& arr) {
  detail::QuickSortImpl(arr, 0, (int)arr.size() - 1);
}
}  // namespace algo::sort::v2