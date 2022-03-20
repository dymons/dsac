#pragma once

#include <iterator>

namespace dsac {

/**
 * Check whether adjacent elements in the range satisfy the condition \c predicate.
 *
 * \returns \c true if adjacent elements in the range satisfy \c predicate, \c false otherwise.
 *
 * \ingroup DsacAlgorithms
 */
template <typename ForwardIt, typename BinaryPredicate>
bool is_sorted(
    /// Iterator to the initial position in the sequence
    ForwardIt begin,
    /// Iterator to the final position in the sequence
    ForwardIt end,
    /// Function to invoke on every pair of elements in the range
    BinaryPredicate predicate)
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