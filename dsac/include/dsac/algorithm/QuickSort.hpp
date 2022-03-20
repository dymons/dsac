#pragma once

#include <vector>

namespace dsac::sort::v1 {
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
  detail::QuickSortImpl(arr, 0, static_cast<int>(arr.size()) - 1);
}
}  // namespace dsac::sort::v1

namespace dsac::sort::v2 {
namespace detail {
template <typename T>
void QuickSortImpl(std::vector<T>& arr, const int low, const int high) {
  // Lomuto partition
  if (low < high) {
    int left = low;
    // Использовать крайние значения, является плохим решением, т.к. для
    // случая 1 2 3 4 5 6 мы получим следующее разбиение:
    // 1 | 2 3 4 5 6
    //     2 | 3 4 5 6
    //         3 | 4 5 6
    //             4 | 5 6
    //                 5 | 6
    // Получаем очень глубокую рекурсию. Лучшим решение является взять медиану
    // const int pivot = median(A1,A2,A3), где:
    // A1 - arr[low]
    // A2 - arr[high]
    // A3 - arr[(high+low) / 2]

    // Интересная статья про поиск median за O(n) в неотсортированном массиве
    // https://habr.com/ru/post/346930/
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
  detail::QuickSortImpl(arr, 0, static_cast<int>(arr.size()) - 1);
}
}  // namespace dsac::sort::v2