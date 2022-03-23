#ifndef ALGORITHM_IS_SORTED_HPP_
#error This file may only be included from IsSorted.hpp
#endif

#include <functional>

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
  return ::dsac::is_sorted(begin, end, std::less<value_type>{});
}

template <typename ForwardRange, typename BinaryPredicate>
bool is_sorted(ForwardRange&& range, BinaryPredicate predicate)
{
  using std::begin;
  using std::end;
  return ::dsac::is_sorted(
      begin(std::forward<ForwardRange>(range)), end(std::forward<ForwardRange>(range)), predicate);
}

template <typename ForwardRange>
bool is_sorted(ForwardRange&& range)
{
  using std::begin;
  using std::end;
  using iterator_type = typename dsac::container_traits<ForwardRange>::iterator_type;
  using value_type    = typename dsac::iterator_traits<iterator_type>::value_type;

  return ::dsac::is_sorted(
      begin(std::forward<ForwardRange>(range)),
      end(std::forward<ForwardRange>(range)),
      std::less<value_type>{});
}

template <typename T>
bool is_sorted(std::initializer_list<T> range)
{
  using std::begin;
  using std::end;
  return ::dsac::is_sorted(begin(range), end(range));
}

template <typename T, typename BinaryPredicate>
bool is_sorted(std::initializer_list<T> range, BinaryPredicate predicate)
{
  using std::begin;
  using std::end;
  return ::dsac::is_sorted(begin(range), end(range), predicate);
}
}  // namespace dsac