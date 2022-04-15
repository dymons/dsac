#pragma once

#include <iterator>

namespace dsac {
template <typename Iterator, typename Container>
class NormalIterator {
private:
  Iterator current_;

public:
  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using difference_type =
      typename std::iterator_traits<Iterator>::difference_type;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using pointer   = typename std::iterator_traits<Iterator>::pointer;

  NormalIterator()
    : current_(Iterator()) {
  }

  explicit NormalIterator(const Iterator& iterator)
    : current_(iterator) {
  }

  template <typename OtherIterator>
  explicit NormalIterator(
      const NormalIterator<
          OtherIterator,
          typename std::enable_if<
              (std::is_same_v<OtherIterator, typename Container::pointer>),
              Container>::type>& iterator)
    : current_(iterator.base()) {
  }

  reference operator*() const {
    return *current_;
  }

  pointer operator->() const {
    return current_;
  }

  NormalIterator& operator++() {
    ++current_;
    return *this;
  }

  NormalIterator operator++(int) {
    return NormalIterator(current_++);
  }

  NormalIterator& operator--() {
    --current_;
    return *this;
  }

  NormalIterator operator--(int) {
    return NormalIterator(current_--);
  }

  reference operator[](const difference_type& n) const {
    return current_[n];
  }

  NormalIterator& operator+=(const difference_type& n) {
    current_ += n;
    return *this;
  }

  NormalIterator operator+(const difference_type& n) const {
    return NormalIterator(current_ + n);
  }

  NormalIterator& operator-=(const difference_type& n) {
    current_ -= n;
    return *this;
  }

  NormalIterator operator-(const difference_type& n) const {
    return NormalIterator(current_ - n);
  }

  const Iterator& base() const {
    return current_;
  }
};

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator==(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() == rha.base();
}

template <typename Iterator, typename Container>
inline bool operator==(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() == rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator!=(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() != rha.base();
}

template <typename Iterator, typename Container>
inline bool operator!=(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() != rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() < rha.base();
}

template <typename Iterator, typename Container>
inline bool operator<(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() < rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() > rha.base();
}

template <typename Iterator, typename Container>
inline bool operator>(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() > rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<=(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() <= rha.base();
}

template <typename Iterator, typename Container>
inline bool operator<=(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() <= rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>=(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() >= rha.base();
}

template <typename Iterator, typename Container>
inline bool operator>=(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() >= rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline typename NormalIterator<IteratorL, Container>::difference_type operator-(
    const NormalIterator<IteratorL, Container>& lhs,
    const NormalIterator<IteratorR, Container>& rha) {
  return lhs.base() - rha.base();
}

template <typename Iterator, typename Container>
inline typename NormalIterator<Iterator, Container>::difference_type operator-(
    const NormalIterator<Iterator, Container>& lhs,
    const NormalIterator<Iterator, Container>& rha) {
  return lhs.base() - rha.base();
}

template <typename Iterator, typename Container>
inline NormalIterator<Iterator, Container> operator+(
    typename NormalIterator<Iterator, Container>::difference_type n,
    const NormalIterator<Iterator, Container>&                    iterator) {
  return NormalIterator<Iterator, Container>(iterator.base() + n);
}
}  // namespace dsac