#pragma once

#include <dsac/algorithm/detail/bubble_sort.hpp>
#include <dsac/type_traits/container_traits.hpp>
#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {

/*!
    \brief
        Make visible internal function for users through the interface dsac::bubble_sort(b, e, p).
*/
using detail::bubble_sort;

/*!
    \brief
        Bubble Sort is the simplest sorting algorithm that works by repeatedly swapping the adjacent
        elements if they are in wrong order.

    \param first
        Iterator to the initial position in the sequence
    \param last
        Iterator to the final position in the sequence

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename RandomIterator>
[[gnu::always_inline]] inline void bubble_sort(RandomIterator first, RandomIterator last)
{
  detail::bubble_sort(first, last, std::less<>{});
}

/*!
    \brief
        Bubble Sort is the simplest sorting algorithm that works by repeatedly swapping the adjacent
        elements if they are in wrong order.

    \param range
        The sequence of elements
    \param comp
        By what criteria should values be sorted in the range

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename RandomRange, typename Compare>
[[gnu::always_inline]] inline void bubble_sort(RandomRange&& range, Compare comp)
{
  using std::begin;
  using std::end;

  return detail::bubble_sort(
      begin(std::forward<RandomRange>(range)), end(std::forward<RandomRange>(range)), comp);
}

/*!
    \brief
        Bubble Sort is the simplest sorting algorithm that works by repeatedly swapping the adjacent
        elements if they are in wrong order.

    \param range
        The sequence of elements

    \par Worst Case Complexity:
        Time Complexity \p O(N^2) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename RandomRange>
[[gnu::always_inline]] inline void bubble_sort(RandomRange&& range)
{
  using std::begin;
  using std::end;

  return ::dsac::bubble_sort(std::forward<RandomRange>(range), std::less<>{});
}

}  // namespace dsac