#pragma once

#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {
template <typename ForwardIterator, typename Compare>
void insertion_sort(ForwardIterator begin, ForwardIterator end, Compare comp)
{
  using value_type      = typename dsac::iterator_traits<ForwardIterator>::value_type;
  using difference_type = typename dsac::iterator_traits<ForwardIterator>::difference_type;

  difference_type const n = std::distance(begin, end);
  for (difference_type j = 1; j < n; ++j) {
    value_type      key = begin[j];
    difference_type i   = j - 1;
    while ((i >= 0) && comp(key, begin[i])) {
      begin[i + 1] = begin[i];
      i            = i - 1;
    }
    begin[i + 1] = key;
  }
}
}  // namespace dsac