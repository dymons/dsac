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
template <typename ForwardIterator>
void bubble_sort(ForwardIterator first, ForwardIterator last)
{
  using value_type = typename dsac::iterator_traits<ForwardIterator>::value_type;
  detail::bubble_sort(first, last, std::less<value_type>{});
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
template <typename ForwardRange, typename Compare>
void bubble_sort(ForwardRange&& range, Compare comp)
{
  using std::begin;
  using std::end;

  return detail::bubble_sort(
      begin(std::forward<ForwardRange>(range)),
      end(std::forward<ForwardRange>(range)),
      std::move(comp));
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
template <typename ForwardRange>
void bubble_sort(ForwardRange&& range)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  return ::dsac::bubble_sort(std::forward<ForwardRange>(range), std::less<value_type>{});
}

}  // namespace dsac