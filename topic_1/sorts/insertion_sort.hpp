#pragma once

#include <cmath>
#include <limits>
#include <utility>
#include <iterator>
#include <algorithm>

namespace inter::sort {
/*
   Псевдокод алгоритма:
   for j = 1 to arr.length do
   key = arr[j]
   i = j-1
   while (int i > 0 and arr[i] > key) do
       arr[i + 1] = arr[i]
       i = i - 1
   end while
   arr[i+1] = key
   end
*/
template <typename T>
void InsertionSort(std::vector<T>& arr) {
  for (int j = 1; j < arr.size(); ++j) {
    int key = arr[j];
    int i = j - 1;
    while ((i >= 0) && (arr[i] > key)) {
      arr[i + 1] = arr[i];
      i = i - 1;
    }
    arr[i + 1] = key;
  }
}

template <class Iterator>
void selection_sort(Iterator t_begin, Iterator t_end) {
  for (Iterator itr = t_begin; itr != t_end; ++itr) {
    auto min = std::min_element(itr, t_end);
    std::swap(*itr, *min);
  }
}

template <class Iterator>
void merge_sort(Iterator t_begin, Iterator t_end) {
  // Recursion ends when the sequence becomes 1
  if (std::distance(t_begin, t_end) < 2) {
    return;
  }

  Iterator middle =
      t_begin + static_cast<std::size_t>((std::distance(t_begin, t_end) / 2));
  merge_sort(t_begin, middle);
  merge_sort(middle, t_end);
  std::merge(t_begin, middle, middle, t_end, t_begin);
}

template <class Iterator>
void bubble_sort(Iterator t_begin, Iterator t_end) {
  for (Iterator itr = t_begin; itr != t_end; ++itr) {
    for (Iterator it = t_begin; it != (t_end - std::distance(t_begin, itr));
         ++it) {
      if (*it > *std::next(it)) {
        std::swap(*it, *std::next(it));
      }
    }
  }
}

template <class Iterator>
void quick_sort(Iterator t_begin, Iterator t_end) {
  // From: https://ru.cppreference.com/w/cpp/algorithm/partition
  if (std::distance(t_begin, t_end) > 1) {
    Iterator middle =
        t_begin + static_cast<std::size_t>((std::distance(t_begin, t_end) / 2));
    Iterator left_middle_it =
        std::partition(t_begin, t_end, [reference_value = *middle](auto& e) {
          return e < reference_value;
        });
    Iterator right_middle_it = std::partition(
        left_middle_it, t_end, [reference_value = *middle](auto& e) {
          return !(reference_value < e);
        });

    quick_sort(t_begin, left_middle_it);
    quick_sort(right_middle_it, t_end);
  }
}
}  // namespace inter::sort