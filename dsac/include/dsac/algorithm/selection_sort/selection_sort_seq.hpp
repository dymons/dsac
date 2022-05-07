#pragma once

#include <dsac/algorithm/detail/selection_sort.hpp>
#include <dsac/type_traits/container_traits.hpp>
#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {

/*!
    \brief
        Make visible internal function for users through the interface
   dsac::selection_sort(b,e,p).
*/
using detail::selection_sort;

/*!
    \brief
        The selection sort algorithm sorts an array by repeatedly finding the
   minimum element.

    \param first
        Iterator to the initial position in the sequence
    \param last
        Iterator to the final position in the sequence

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup
        DsacAlgorithms
*/
template <std::random_access_iterator RandomIterator>
[[gnu::always_inline]] inline void selection_sort(
    RandomIterator first, RandomIterator last) {
  detail::selection_sort(first, last, std::less<>{});
}

/*!
    \brief
        The selection sort algorithm sorts an array by repeatedly finding the
   minimum element.

    \param range
        The sequence of elements
    \param comp
        By what criteria should values be sorted in the range

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup
        DsacAlgorithms
*/
template <typename RandomRange, typename Compare>
[[gnu::always_inline]] inline void selection_sort(
    RandomRange&& range, Compare comp) {
  using std::begin;
  using std::end;

  return detail::selection_sort(
      begin(std::forward<RandomRange>(range)),
      end(std::forward<RandomRange>(range)),
      comp);
}

/*!
    \brief
        The selection sort algorithm sorts an array by repeatedly finding the
   minimum element.

    \param range
        The sequence of elements

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup
        DsacAlgorithms
*/
template <typename RandomRange>
[[gnu::always_inline]] inline void selection_sort(RandomRange&& range) {
  return ::dsac::selection_sort(
      std::forward<RandomRange>(range), std::less<>{});
}

}  // namespace dsac