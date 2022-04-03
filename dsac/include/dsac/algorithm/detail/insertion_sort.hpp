#pragma once

#include "dsac/type_traits/iterator_traits.hpp"

namespace dsac::detail {
/*!
    \brief
        Insertion sort is a sorting algorithm that places an unsorted element at its suitable place
        in each iteration.

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
template <typename RandomIterator, typename Compare>
void insertion_sort(RandomIterator begin, RandomIterator end, Compare comp)
{
  using value_type      = typename dsac::iterator_traits<RandomIterator>::value_type;
  using difference_type = typename dsac::iterator_traits<RandomIterator>::difference_type;

  static_assert(std::is_signed_v<difference_type>, "difference type must be signed");

  difference_type const n = std::distance(begin, end);
  for (difference_type j = 1; j < n; ++j) {
    value_type      key = begin[j];
    difference_type i   = j - 1;
    while ((i >= 0) && comp(key, begin[i])) {
      begin[i + 1] = begin[i];
      i            = i - 1;
    }
    begin[i + 1] = key;
  }
}
}  // namespace dsac::detail