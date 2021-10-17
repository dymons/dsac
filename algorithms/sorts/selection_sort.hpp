#pragma once

#include <cmath>
#include <limits>
#include <utility>
#include <iterator>
#include <algorithm>

namespace algo::sort {
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
}  // namespace algo::sort