#pragma once

#include <dsac/algorithm/datail/binary_tree_sort.hpp>
#include <dsac/container/tree/bs_tree.hpp>
#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {
template <typename ForwardIterator>
void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  using value_type          = typename dsac::iterator_traits<ForwardIterator>::value_type;
  using binary_tree_politic = binary_search_tree<value_type>;
  detail::binary_tree_sort<binary_tree_politic>(first, last);
}
}  // namespace dsac

