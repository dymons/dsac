#pragma once

#include <iterator>

namespace dsac {
template <typename Iterator, typename Container>
class NormalIterator {
protected:
  Iterator current_;

public:
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type        = typename std::iterator_traits<Iterator>::value_type;
  using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
  using reference         = typename std::iterator_traits<Iterator>::reference;
  using pointer           = typename std::iterator_traits<Iterator>::pointer;

  NormalIterator()
    : current_(Iterator())
  {
  }

  explicit NormalIterator(const Iterator& __i)
    : current_(__i)
  {
  }

  // Allow iterator to constIterator conversion
  template <typename _Iter>
  explicit NormalIterator(const NormalIterator<
                          _Iter,
                          typename std::enable_if<
                              (std::__are_same<_Iter, typename Container::pointer>::__value),
                              Container>::__type>& __i)
    : current_(__i.base())
  {
  }

  // Forward iterator requirements
  reference operator*() const
  {
    return *current_;
  }

  pointer operator->() const
  {
    return current_;
  }

  NormalIterator& operator++()
  {
    ++current_;
    return *this;
  }

  NormalIterator operator++(int)
  {
    return NormalIterator(current_++);
  }

  NormalIterator& operator--()
  {
    --current_;
    return *this;
  }

  NormalIterator operator--(int)
  {
    return NormalIterator(current_--);
  }

  reference operator[](const difference_type& __n) const
  {
    return current_[__n];
  }

  NormalIterator& operator+=(const difference_type& __n)
  {
    current_ += __n;
    return *this;
  }

  NormalIterator operator+(const difference_type& __n) const
  {
    return NormalIterator(current_ + __n);
  }

  NormalIterator& operator-=(const difference_type& __n)
  {
    current_ -= __n;
    return *this;
  }

  NormalIterator operator-(const difference_type& __n) const
  {
    return NormalIterator(current_ - __n);
  }

  const Iterator& base() const
  {
    return current_;
  }
};

// Forward iterator requirements
template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator==(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() == __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator==(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() == __rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator!=(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() != __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator!=(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() != __rhs.base();
}

// Random access iterator requirements
template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() < __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator<(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() < __rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() > __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator>(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() > __rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<=(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() <= __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator<=(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() <= __rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>=(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() >= __rhs.base();
}

template <typename Iterator, typename Container>
inline bool operator>=(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() >= __rhs.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline typename NormalIterator<IteratorL, Container>::difference_type operator-(
    const NormalIterator<IteratorL, Container>& __lhs,
    const NormalIterator<IteratorR, Container>& __rhs)
{
  return __lhs.base() - __rhs.base();
}

template <typename Iterator, typename Container>
inline typename NormalIterator<Iterator, Container>::difference_type operator-(
    const NormalIterator<Iterator, Container>& __lhs,
    const NormalIterator<Iterator, Container>& __rhs)
{
  return __lhs.base() - __rhs.base();
}

template <typename Iterator, typename Container>
inline NormalIterator<Iterator, Container> operator+(
    typename NormalIterator<Iterator, Container>::difference_type __n,
    const NormalIterator<Iterator, Container>&                    __i)
{
  return NormalIterator<Iterator, Container>(__i.base() + __n);
}
}  // namespace dsac