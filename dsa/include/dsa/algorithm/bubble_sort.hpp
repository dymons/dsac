#pragma once

#include <vector>

namespace algo::sort {
template <typename T>
void BubbleSort(std::vector<T>& arr) {
  for (int j = 0; j < arr.size(); ++j) {
    for (int i = 0; i < (arr.size() - 1 - j); ++i) {
      if (arr[i] > arr[i + 1]) {
        std::swap(arr[i], arr[i + 1]);
      }
    }
  }
}
}  // namespace algo::sort