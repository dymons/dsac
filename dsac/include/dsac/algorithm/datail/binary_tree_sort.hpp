#pragma once

#include <algorithm>

namespace dsac::detail {

/*!
    \brief
        Tree sort is a sorting algorithm that is based on Binary Search Tree data structure.

    \param first
        Iterator to the initial position in the sequence
    \param last
        Iterator to the final position in the sequence

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup DsacAlgorithms
*/
template <class BinaryTreePolicy, typename ForwardIterator>
void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  using std::begin;
  using std::end;

  BinaryTreePolicy tree(first, last);
  std::copy(begin(tree), end(tree), first);
}
}  // namespace dsac::detail
