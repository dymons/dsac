#pragma once

#include <vector>

namespace dsac::sort {
template <typename T>
void ShellSort(std::vector<T>& arr) {
  for (int d = arr.size() / 2; d > 0; d /= 2) {
    for (int step = d; step < arr.size(); ++step) {
      for (int j = step; j >= d; j -= d) {
        if (arr[j] < arr[j - d]) {
          std::swap(arr[j], arr[j - d]);
        }
      }
    }
  }
}
}  // namespace dsac::sort