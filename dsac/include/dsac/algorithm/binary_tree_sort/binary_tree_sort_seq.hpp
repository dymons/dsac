#pragma once

#include <dsac/algorithm/datail/binary_tree_sort.hpp>
#include <dsac/container/tree/bs_tree.hpp>
#include <dsac/type_traits/iterator_traits.hpp>
#include "dsac/type_traits/container_traits.hpp"

namespace dsac {

using detail::binary_tree_sort;

template <typename ForwardIterator>
void binary_tree_sort(ForwardIterator first, ForwardIterator last)
{
  using value_type         = typename dsac::iterator_traits<ForwardIterator>::value_type;
  using binary_tree_policy = binary_search_tree<value_type>;
  detail::binary_tree_sort(first, last, binary_tree_policy{});
}

template <typename ForwardRange, typename Compare>
void binary_tree_sort(ForwardRange&& range, Compare comp)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  binary_search_tree<value_type, Compare> binary_tree{std::move(comp)};
  return detail::binary_tree_sort(
      begin(std::forward<ForwardRange>(range)),
      end(std::forward<ForwardRange>(range)),
      std::move(binary_tree));
}

template <typename ForwardRange>
void binary_tree_sort(ForwardRange&& range)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  return ::dsac::binary_tree_sort(std::forward<ForwardRange>(range), std::less<value_type>{});
}

}  // namespace dsac

