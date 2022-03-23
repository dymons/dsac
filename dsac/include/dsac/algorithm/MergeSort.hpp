#pragma once

#include <vector>

namespace dsac::sort {
namespace detail {
template <typename T>
void Merge(std::vector<T>& arr, const int low, const int mid, const int high)
{
  const int n1 = mid - low + 1;
  const int n2 = high - mid;

  std::vector<T> L(n1);
  std::vector<T> R(n2);

  for (int i = 0; i < n1; ++i) {
    L[i] = arr[low + i];
  }

  for (int i = 0; i < n2; ++i) {
    R[i] = arr[mid + i + 1];
  }

  int i = 0;
  int j = 0;
  int k = low;
  for (; k <= high; ++k) {
    if ((i == L.size()) || (j == R.size())) {
      break;
    }

    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i      = i + 1;
    }
    else {
      arr[k] = R[j];
      j      = j + 1;
    }
  }

  if (i == L.size()) {
    for (; j < R.size(); ++j, ++k) {
      arr[k] = R[j];
    }
  }

  if (j == R.size()) {
    for (; i < L.size(); ++i, ++k) {
      arr[k] = L[i];
    }
  }
}

template <typename T>
void MergeSortTopDownImpl(std::vector<T>& arr, const int low, const int high)
{
  if (low < high) {
    const int middle = (low + high) / 2;
    MergeSortTopDownImpl(arr, low, middle);
    MergeSortTopDownImpl(arr, middle + 1, high);
    Merge(arr, low, middle, high);
  }
}

// TODO: Добавить версию сортировки Merge Sort Bottom-Up
}  // namespace detail

template <typename T>
void MergeSort(std::vector<T>& arr)
{
  // Worst Case Time Complexity [ Big-O ] : O(NlogN)
  // Best Case Time Complexity [Big-omega]: O(NlogN)
  // Average Time Complexity [Big-theta]  : O(NlogN)
  // Space Complexity : O(N), но можно лучше (O(N/2)), если копировать одну половину
  //                          массива и сохраняться отсортированные значения In Place
  detail::MergeSortTopDownImpl(arr, 0, static_cast<int>(arr.size()) - 1);
}
}  // namespace dsac::is_sorted