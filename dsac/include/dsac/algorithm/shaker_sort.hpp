#pragma once

#include <vector>

namespace dsac::sort::v1 {
template <typename T>
void ShakerSort(std::vector<T>& arr) {
  int left  = 0;
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
}  // namespace dsac::sort::v1

namespace dsac::sort::v2 {
template <typename T>
void ShakerSort(std::vector<T>& arr) {
  int left        = 0;
  int right       = (int)arr.size() - 1;
  int latest_swap = (int)arr.size() - 1;
  do {
    for (int i = left; i < right; ++i) {
      if (arr[i] > arr[i + 1]) {
        std::swap(arr[i], arr[i + 1]);
        latest_swap = i;
      }
    }
    right = latest_swap;

    for (int i = right; i > left; --i) {
      if (arr[i - 1] > arr[i]) {
        std::swap(arr[i - 1], arr[i]);
        latest_swap = i;
      }
    }
    left = latest_swap;
  } while (left < right);
}
}  // namespace dsac::sort::v2