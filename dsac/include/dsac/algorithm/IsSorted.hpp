#pragma once

#include <iterator>

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
template <typename ForwardIt, typename BinaryPredicate>
bool is_sorted(ForwardIt begin, ForwardIt end, BinaryPredicate predicate)
{
  if (begin == end) {
    return true;
  }

  ForwardIt next = begin;
  for (++next; next != end; begin = next, (void)++next) {
    if (predicate(*next, *begin)) {
      return false;
    }
  }

  return true;
}

}  // namespace dsac