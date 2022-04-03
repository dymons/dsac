#pragma once

#include <dsac/algorithm/detail/binary_tree_sort.hpp>
#include <dsac/container/tree/bs_tree.hpp>
#include <dsac/type_traits/iterator_traits.hpp>
#include "dsac/type_traits/container_traits.hpp"

namespace dsac {

/*!
    \brief
        Make visible internal function for users through
        the interface dsac::binary_tree_sort(b, e, politic{}).
*/
using detail::binary_tree_sort;

/*!
    \brief
        Tree sort is a sorting algorithm that is based on Binary Search Tree data structure.
        By default use Binary Search Tree for sorting range of elements.

    \param first
        Iterator to the initial position in the sequence
    \param last
        Iterator to the final position in the sequence

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup DsacAlgorithms
*/
template <typename ForwardIterator>
[[gnu::always_inline]] inline void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  using value_type         = typename dsac::iterator_traits<ForwardIterator>::value_type;
  using binary_tree_policy = binary_search_tree<value_type>;
  detail::binary_tree_sort(first, last, binary_tree_policy{});
}

/*!
    \brief
        Tree sort is a sorting algorithm that is based on Binary Search Tree data structure.
        Sort the range of elements based on the condition \c comp. By default use Binary Search Tree
        for sorting range of elements.

    \param range
        The sequence of elements
    \param comp
        By what criteria should values be sorted in the range

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup DsacAlgorithms
*/
template <typename ForwardRange, typename Compare>
[[gnu::always_inline]] inline void binary_tree_sort(ForwardRange&& range, Compare comp)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  binary_search_tree<value_type, Compare> binary_tree{std::move(comp)};
  return detail::binary_tree_sort(
      begin(std::forward<ForwardRange>(range)),
      end(std::forward<ForwardRange>(range)),
      std::move(binary_tree));
}

/*!
    \brief
        Tree sort is a sorting algorithm that is based on Binary Search Tree data structure.
        By default use Binary Search Tree for sorting range of elements.

    \param range
        The sequence of elements

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup DsacAlgorithms
*/
template <typename ForwardRange>
[[gnu::always_inline]] inline void binary_tree_sort(ForwardRange&& range)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  return ::dsac::binary_tree_sort(std::forward<ForwardRange>(range), std::less<value_type>{});
}

}  // namespace dsac
