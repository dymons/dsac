#pragma once

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace dsac {

class BinaryTreeSortException : public std::range_error {
public:
  using std::range_error::range_error;
};

class DublicatesNotSupported : public BinaryTreeSortException {
public:
  DublicatesNotSupported()
    : BinaryTreeSortException("Work with duplicates is not supported yet")
  {
  }
};

}  // namespace dsac

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
  BinaryTreePolicy binary_tree(first, last);
  auto const       range_size = std::distance(first, last);
  if (binary_tree.size() != range_size) {
    throw DublicatesNotSupported{};
  }

  using std::begin;
  using std::end;
  std::copy(begin(binary_tree), end(binary_tree), first);
}

}  // namespace dsac::detail
