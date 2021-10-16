#pragma once

#include <vector>

namespace algo::sort::v1 {
template <typename T>
void ShakerSort(std::vector<T>& arr) {
  int left = 0;
  int right = (int)arr.size() - 1;
  do {
    for (int i = left; i < right; ++i) {
      if (arr[i] > arr[i + 1]) {
        std::swap(arr[i], arr[i + 1]);
      }
    }
    --right;

    for (int i = right; i > left; --i) {
      if (arr[i - 1] > arr[i]) {
        std::swap(arr[i - 1], arr[i]);
      }
    }
    ++left;
  } while (left < right);
}
}  // namespace algo::sort::v1

namespace algo::sort::v2 {
template <typename T>
void ShakerSort(std::vector<T>& arr) {
  int left = 0;
  int right = (int)arr.size() - 1;
  int control = (int)arr.size() - 1;
  do {
    for (int i = left; i < right; ++i) {
      if (arr[i] > arr[i + 1]) {
        std::swap(arr[i], arr[i + 1]);
        control = i;
      }
    }
    right = control;

    for (int i = right; i > left; --i) {
      if (arr[i - 1] > arr[i]) {
        std::swap(arr[i - 1], arr[i]);
        control = i;
      }
    }
    left = control;
  } while (left < right);
}
}  // namespace algo::sort::v2