#ifndef ALGORITHM_IS_SORTED_HPP_
#error This file may only be included from IsSorted.hpp
#endif

namespace dsac {
template <typename ForwardIterator, typename BinaryPredicate>
bool is_sorted(ForwardIterator begin, ForwardIterator end, BinaryPredicate predicate)
{
  if (begin == end) {
    return true;
  }

  ForwardIterator next = begin;
  for (++next; next != end; begin = next, (void)++next) {
    if (predicate(*next, *begin)) {
      return false;
    }
  }

  return true;
}

template <typename ForwardIterator>
bool is_sorted(ForwardIterator begin, ForwardIterator end)
{
  using value_type = typename dsac::iterator_traits<ForwardIterator>::value_type;
  return ::dsac::is_sorted(begin, end, dsac::less<value_type>{});
}

template <typename Range, typename BinaryPredicate>
bool is_sorted(Range const& range, BinaryPredicate predicate)
{
  return ::dsac::is_sorted(dsac::begin(range), dsac::end(range), predicate);
}

template <typename Range>
bool is_sorted(Range const& range)
{
  using iterator_type = typename dsac::container_traits<Range>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;
  return ::dsac::is_sorted(dsac::begin(range), dsac::end(range), dsac::less<value_type>{});
}
}  // namespace dsac