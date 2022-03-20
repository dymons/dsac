#pragma once

#include <dsac/traits/ContainerTraits.hpp>
#include <dsac/traits/IteratorTraits.hpp>
#include <dsac/util/Functional.hpp>

namespace dsac {

/**
 * \brief   Check whether adjacent elements in the range satisfy the condition \c predicate.
 *
 * \param   begin   Iterator to the initial position in the sequence
 * \param   end   Iterator to the final position in the sequence
 * \param   predicate   Function to invoke on adjacent pair of elements in the range
 *
 * \returns \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.
 *
 * \par     Worst Case Complexity:
 *          Time Complexity \p O(N) and Space Complexity \p O(1)
 *
 * \ingroup DsacAlgorithms
 */
template <typename ForwardIterator, typename BinaryPredicate>
bool is_sorted(ForwardIterator begin, ForwardIterator end, BinaryPredicate predicate)
{
  if (begin == end) {
    return true;
  }

  ForwardIterator next = begin;
  for (++next; next != end; begin = next, (void)++next) {
    if (predicate(*next, *begin)) {
      return false;
    }
  }

  return true;
}

/**
 * \brief   Check whether adjacent elements in the range satisfy the condition \c predicate.
 *
 * \param   begin   Iterator to the initial position in the sequence
 * \param   end   Iterator to the final position in the sequence
 *
 * \returns \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.
 *
 * \par     Worst Case Complexity:
 *          Time Complexity \p O(N) and Space Complexity \p O(1)
 *
 * \ingroup DsacAlgorithms
 */
template <typename ForwardIterator>
[[gnu::always_inline]] inline bool is_sorted(ForwardIterator begin, ForwardIterator end)
{
  using value_type = typename dsac::iterator_traits<ForwardIterator>::value_type;
  return ::dsac::is_sorted(begin, end, dsac::less<value_type>{});
}

/**
 * \brief   Check whether adjacent elements in the range satisfy the condition \c predicate.
 *
 * \param   range   The sequence of elements
 * \param   predicate   Function to invoke on adjacent pair of elements in the range
 *
 * \returns \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.
 *
 * \par     Worst Case Complexity:
 *          Time Complexity \p O(N) and Space Complexity \p O(1)
 *
 * \ingroup DsacAlgorithms
 */
template <typename Range, typename BinaryPredicate>
[[gnu::always_inline]] inline bool is_sorted(Range const& range, BinaryPredicate predicate)
{
  return ::dsac::is_sorted(dsac::begin(range), dsac::end(range), predicate);
}

/**
 * \brief   Check whether adjacent elements in the range satisfy the condition \c predicate.
 *
 * \param   range   The sequence of elements
 *
 * \returns \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.
 *
 * \par     Worst Case Complexity:
 *          Time Complexity \p O(N) and Space Complexity \p O(1)
 *
 * \ingroup DsacAlgorithms
 */
template <typename Range>
[[gnu::always_inline]] inline bool is_sorted(Range const& range)
{
  using iterator_type = typename dsac::container_traits<Range>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;
  return ::dsac::is_sorted(dsac::begin(range), dsac::end(range), dsac::less<value_type>{});
}

}  // namespace dsac