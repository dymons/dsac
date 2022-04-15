#pragma once

#include <dsac/algorithm/detail/insertion_sort.hpp>
#include <dsac/type_traits/container_traits.hpp>
#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {

/*!
    \brief
        Make visible internal function for users through the interface
   dsac::insertion_sort(b,e,p).
*/
using detail::insertion_sort;

/*!
    \brief
        Insertion sort is a sorting algorithm that places an unsorted element at
   its suitable place in each iteration.

    \param first
        Iterator to the initial position in the sequence
    \param last
        Iterator to the final position in the sequence

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <std::random_access_iterator RandomIterator>
[[gnu::always_inline]] inline void insertion_sort(
    RandomIterator first, RandomIterator last) {
  detail::insertion_sort(first, last, std::less<>{});
}

/*!
    \brief
        Insertion sort is a sorting algorithm that places an unsorted element at
   its suitable place in each iteration.

    \param range
        The sequence of elements
    \param comp
        By what criteria should values be sorted in the range

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename RandomRange, typename Compare>
[[gnu::always_inline]] inline void insertion_sort(
    RandomRange&& range, Compare comp) {
  using std::begin;
  using std::end;

  return detail::insertion_sort(
      begin(std::forward<RandomRange>(range)),
      end(std::forward<RandomRange>(range)),
      comp);
}

/*!
    \brief
        Insertion sort is a sorting algorithm that places an unsorted element at
   its suitable place in each iteration.

    \param range
        The sequence of elements

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename RandomRange>
[[gnu::always_inline]] inline void insertion_sort(RandomRange&& range) {
  return ::dsac::insertion_sort(
      std::forward<RandomRange>(range), std::less<>{});
}

}  // namespace dsac