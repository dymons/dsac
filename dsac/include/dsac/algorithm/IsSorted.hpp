#pragma once

#include <dsac/type_traits/ContainerTraits.hpp>
#include <dsac/type_traits/IteratorTraits.hpp>

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
bool is_sorted(ForwardIterator begin, ForwardIterator end, BinaryPredicate predicate);

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
[[gnu::always_inline]] inline bool is_sorted(ForwardIterator begin, ForwardIterator end);

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
[[gnu::always_inline]] inline bool is_sorted(Range const& range, BinaryPredicate predicate);

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
[[gnu::always_inline]] inline bool is_sorted(Range const& range);

}  // namespace dsac

#define ALGORITHM_IS_SORTED_HPP_
#include <dsac/algorithm/internal/IsSorted-inl.hpp>
#undef ALGORITHM_IS_SORTED_HPP_