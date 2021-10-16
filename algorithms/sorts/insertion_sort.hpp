#pragma once

#include <vector>

namespace algo::sort {
template <typename T>
void InsertionSort(std::vector<T>& arr) {
  for (int j = 1; j < arr.size(); ++j) {
    T key = arr[j];
    int i = j - 1;
    while ((i >= 0) && (arr[i] > key)) {
      arr[i + 1] = arr[i];
      i = i - 1;
    }
    arr[i + 1] = key;
  }
}
}  // namespace inter::sort