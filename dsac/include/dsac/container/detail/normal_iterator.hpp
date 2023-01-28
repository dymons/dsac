#pragma once

#include <iterator>

namespace dsac {

template <typename Iterator, typename Container>
class normal_iterator {
private:
  Iterator current_;

public:
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type        = typename std::iterator_traits<Iterator>::value_type;
  using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
  using reference         = typename std::iterator_traits<Iterator>::reference;
  using pointer           = typename std::iterator_traits<Iterator>::pointer;

  normal_iterator()
    : current_(Iterator()) {
  }

  explicit normal_iterator(const Iterator& iterator)
    : current_(iterator) {
  }

  template <typename OtherIterator>
  explicit normal_iterator(
      const normal_iterator<
          OtherIterator,
          typename std::enable_if<(std::is_same_v<OtherIterator, typename Container::pointer>), Container>::type>&
          iterator
  )
    : current_(iterator.base()) {
  }

  reference operator*() const {
    return *current_;
  }

  pointer operator->() const {
    return current_;
  }

  normal_iterator& operator++() {
    ++current_;
    return *this;
  }

  normal_iterator operator++(int) {
    return normal_iterator(current_++);
  }

  normal_iterator& operator--() {
    --current_;
    return *this;
  }

  normal_iterator operator--(int) {
    return normal_iterator(current_--);
  }

  reference operator[](const difference_type& n) const {
    return current_[n];
  }

  normal_iterator& operator+=(const difference_type& n) {
    current_ += n;
    return *this;
  }

  normal_iterator operator+(const difference_type& n) const {
    return normal_iterator(current_ + n);
  }

  normal_iterator& operator-=(const difference_type& n) {
    current_ -= n;
    return *this;
  }

  normal_iterator operator-(const difference_type& n) const {
    return normal_iterator(current_ - n);
  }

  const Iterator& base() const {
    return current_;
  }
};

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator==(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() == rha.base();
}

template <typename Iterator, typename Container>
inline bool operator==(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() == rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator!=(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() != rha.base();
}

template <typename Iterator, typename Container>
inline bool operator!=(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() != rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() < rha.base();
}

template <typename Iterator, typename Container>
inline bool operator<(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() < rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() > rha.base();
}

template <typename Iterator, typename Container>
inline bool operator>(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() > rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator<=(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() <= rha.base();
}

template <typename Iterator, typename Container>
inline bool operator<=(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() <= rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline bool operator>=(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() >= rha.base();
}

template <typename Iterator, typename Container>
inline bool operator>=(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() >= rha.base();
}

template <typename IteratorL, typename IteratorR, typename Container>
inline typename normal_iterator<IteratorL, Container>::difference_type operator-(
    const normal_iterator<IteratorL, Container>& lhs, const normal_iterator<IteratorR, Container>& rha
) {
  return lhs.base() - rha.base();
}

template <typename Iterator, typename Container>
inline typename normal_iterator<Iterator, Container>::difference_type operator-(
    const normal_iterator<Iterator, Container>& lhs, const normal_iterator<Iterator, Container>& rha
) {
  return lhs.base() - rha.base();
}

template <typename Iterator, typename Container>
inline normal_iterator<Iterator, Container> operator+(
    typename normal_iterator<Iterator, Container>::difference_type n,
    const normal_iterator<Iterator, Container>&                    iterator
) {
  return normal_iterator<Iterator, Container>(iterator.base() + n);
}

}  // namespace dsac
