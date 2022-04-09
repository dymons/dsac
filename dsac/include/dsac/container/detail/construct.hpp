#pragma once

#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {
template <std::forward_iterator ForwardIterator, typename Allocator>
void destroy(ForwardIterator first, ForwardIterator last, Allocator& allocator)
{
  using allocator_traits = typename std::allocator_traits<Allocator>;
  for (; first != last; ++first) {
    allocator_traits::destroy(allocator, std::addressof(*first));
  }
}

template <typename InputIt, typename NoThrowForwardIt, typename Allocator>
NoThrowForwardIt uninitialized_copy(
    InputIt first, InputIt last, NoThrowForwardIt d_first, Allocator& allocator)
{
  using value_type       = typename std::iterator_traits<NoThrowForwardIt>::value_type;
  using allocator_traits = typename std::allocator_traits<Allocator>;

  NoThrowForwardIt current = d_first;
  try {
    for (; first != last; (void)++first, (void)++current) {
      allocator_traits::construct(allocator, current, *first);
    }
  }
  catch (...) {
    dsac::destroy(d_first, current, allocator);
    throw;
  }
  return current;
}

template <typename InputIt, typename NoThrowForwardIt, typename Allocator>
NoThrowForwardIt uninitialized_move(
    InputIt first, InputIt last, NoThrowForwardIt d_first, Allocator& allocator)
{
  using value_type       = typename std::iterator_traits<NoThrowForwardIt>::value_type;
  using allocator_traits = typename std::allocator_traits<Allocator>;

  NoThrowForwardIt current = d_first;
  try {
    for (; first != last; (void)++first, (void)++current) {
      allocator_traits::construct(allocator, current, std::move(*first));
    }
  }
  catch (...) {
    dsac::destroy(d_first, current, allocator);
    throw;
  }
  return current;
}

template <typename InputIt, typename NoThrowForwardIt, typename Allocator>
NoThrowForwardIt uninitialized_move_if_noexcept(
    InputIt first, InputIt last, NoThrowForwardIt d_first, Allocator& allocator)
{
  using value_type = typename dsac::iterator_traits<NoThrowForwardIt>::value_type;

  constexpr bool kIsNothrowMoveConstruct = std::is_nothrow_move_constructible_v<value_type>;
  constexpr bool kIsCopyConstructible    = std::is_copy_constructible_v<value_type>;
  if constexpr (!kIsNothrowMoveConstruct && kIsCopyConstructible) {
    return dsac::uninitialized_copy(first, last, d_first, allocator);
  }

  return dsac::uninitialized_move(first, last, d_first, allocator);
}
}  // namespace dsac