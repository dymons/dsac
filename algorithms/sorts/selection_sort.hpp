#pragma once

#include <vector>
namespace algo::sort {
template <typename T>
void SelectionSort(std::vector<T>& arr) {
  for (int i = 0; i < arr.size(); ++i) {
    int min = i;
    for (int j = i + 1; j < arr.size(); ++j) {
      if (arr[j] < arr[min]) {
        min = j;
      }
    }
    std::swap(arr[i], arr[min]);
  }
}
}  // namespace algo::sort