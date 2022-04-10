#pragma once

#include <dsac/type_traits/iterator_traits.hpp>

namespace dsac {

template <typename Pointer, typename Allocator, typename... Args>
[[gnu::always_inline]] inline void construct(Pointer pointer, Allocator& allocator, Args&&... args)
{
  using allocator_traits = typename std::allocator_traits<Allocator>;
  allocator_traits::construct(allocator, pointer, std::forward<Args>(args)...);
}

template <std::forward_iterator ForwardIterator, typename Allocator>
void destroy(ForwardIterator first, ForwardIterator last, Allocator& allocator)
{
  using allocator_traits = typename std::allocator_traits<Allocator>;
  for (; first != last; ++first) {
    allocator_traits::destroy(allocator, std::addressof(*first));
  }
}

template <typename Allocator>
[[nodiscard, gnu::always_inline]] inline auto allocate(size_t n, Allocator& allocator)
{
  using allocator_traits = typename std::allocator_traits<Allocator>;
  using pointer          = typename allocator_traits::pointer;
  return n != 0 ? allocator_traits::allocate(allocator, n) : pointer{};
}

template <typename Pointer, typename Allocator>
[[gnu::always_inline]] inline void deallocate(Pointer pointer, size_t n, Allocator& allocator)
{
  using allocator_traits = std::allocator_traits<Allocator>;
  if (pointer) {
    allocator_traits::deallocate(allocator, pointer, n);
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