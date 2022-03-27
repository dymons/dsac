#pragma once

#include <dsac/algorithm/detail/is_sorted.hpp>

namespace dsac {

/*!
    \brief
        Make visible internal function for users through the interface dsac::is_sorted(b, e, p).
*/
using detail::is_sorted;

/*!
    \brief
        Check whether adjacent elements in the range satisfy the condition \c predicate.

    \param begin
        Iterator to the initial position in the sequence
    \param end
        Iterator to the final position in the sequence

    \returns
        \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename ForwardIterator>
[[gnu::always_inline]] constexpr inline bool is_sorted(ForwardIterator begin, ForwardIterator end)
{
  using value_type = typename dsac::iterator_traits<ForwardIterator>::value_type;
  return detail::is_sorted(begin, end, std::less<value_type>{});
}

/*!
    \brief
        Check whether adjacent elements in the range satisfy the condition \c comp.

    \param range
        The sequence of elements
    \param comp
        Function to invoke on adjacent pair of elements in the range

    \returns
        \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename ForwardRange, typename Compare>
[[gnu::always_inline]] constexpr inline bool is_sorted(ForwardRange&& range, Compare comp)
{
  using std::begin;
  using std::end;
  return detail::is_sorted(
      begin(std::forward<ForwardRange>(range)), end(std::forward<ForwardRange>(range)), comp);
}

/*!
    \brief
        Check whether adjacent elements in the range satisfy the condition std::less.

    \param range
        The sequence of elements

    \returns
        \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename ForwardRange>
[[gnu::always_inline]] constexpr inline bool is_sorted(ForwardRange&& range)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  return detail::is_sorted(
      begin(std::forward<ForwardRange>(range)),
      end(std::forward<ForwardRange>(range)),
      std::less<value_type>{});
}

/*!
    \brief
        Check whether adjacent elements in the range satisfy the condition \c predicate.

    \param range
        The sequence of elements

    \returns
        \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename T>
[[gnu::always_inline]] constexpr inline bool is_sorted(std::initializer_list<T> range)
{
  using std::begin;
  using std::end;
  return ::dsac::is_sorted(begin(range), end(range));
}

/*!
    \brief
        Check whether adjacent elements in the range satisfy the condition \c predicate.

    \param range
        The sequence of elements
    \param comp
        Function to invoke on adjacent pair of elements in the range

    \returns
        \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(1)

    \ingroup DsacAlgorithms
*/
template <typename T, typename Compare>
[[gnu::always_inline]] constexpr inline bool is_sorted(std::initializer_list<T> range, Compare comp)
{
  using std::begin;
  using std::end;
  return ::dsac::is_sorted(begin(range), end(range), comp);
}
}  //  namespace dsac
