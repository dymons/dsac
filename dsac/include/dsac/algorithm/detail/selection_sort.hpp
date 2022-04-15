#pragma once

#include <algorithm>

namespace dsac::detail {
/*!
    \brief
        The selection sort algorithm sorts an array by repeatedly finding the
   minimum element.

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
void selection_sort(RandomIterator begin, RandomIterator end, Compare comp) {
  for (RandomIterator i = begin; i < end; ++i) {
    RandomIterator min = i;
    for (RandomIterator j = i + 1; j < end; ++j) {
      if (comp(*j, *min)) {
        min = j;
      }
    }
    std::iter_swap(i, min);
  }
}
}  // namespace dsac::detail