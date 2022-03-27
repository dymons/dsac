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

    \tparam BinaryTreePolicy
        A policy class that allows you to combine different behaviors to create a rich variety of
   binary sorting algorithm combinations. \param first Iterator to the initial position in the
   sequence \param last Iterator to the final position in the sequence

    \throw DublicatesNotSupported
        The algorithm does not support working with duplicate values at the moment

    \par Worst Case Complexity:
        Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup DsacAlgorithms

    \code
        using binary_tree_policy = dsac::binary_search_tree<int>;
        dsac::binary_tree_sort<binary_tree_policy>(begin(testcase), end(testcase));

        using binary_tree_policy = dsac::red_black_tree<int, std::greater<int>>;
        dsac::binary_tree_sort<binary_tree_policy>(begin(testcase), end(testcase));
    \endcode
*/
template <class BinaryTreePolicy, typename ForwardIterator>
void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  BinaryTreePolicy binary_tree(first, last);
  if (binary_tree.size() != std::distance(first, last)) {
    throw DublicatesNotSupported{};
  }

  using std::begin;
  using std::end;
  std::copy(begin(binary_tree), end(binary_tree), first);
}

}  // namespace dsac::detail
