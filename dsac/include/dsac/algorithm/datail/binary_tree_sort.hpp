#pragma once

#include <algorithm>

namespace dsac::detail {
template <class BinaryTreePolitic, typename ForwardIterator>
void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  using std::begin;
  using std::end;

  BinaryTreePolitic tree(first, last);
  std::copy(begin(tree), end(tree), first);
}
}  // namespace dsac::detail
