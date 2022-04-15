#pragma once

#include <algorithm>

namespace dsac::detail {
/*!
    \brief
        Bubble Sort is the simplest sorting algorithm that works by repeatedly
   swapping the adjacent elements if they are in wrong order.

    \param begin
        Iterator to the initial position in the sequence
    \param end
        Iterator to the final position in the sequence
    \param comp
        By what criteria should values be sorted in the range

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <std::random_access_iterator RandomIterator, typename Compare>
void bubble_sort(RandomIterator begin, RandomIterator end, Compare comp) {
  for (RandomIterator i = begin; i != end; ++i) {
    for (RandomIterator j = begin; j < i; ++j) {
      if (comp(*i, *j)) {
        std::iter_swap(i, j);
      }
    }
  }
}
}  // namespace dsac::detail