#pragma once

namespace dsac {
template <typename Range>
struct container_traits {
  using iterator_type = typename Range::iterator;
};

template <typename Range>
struct container_traits<Range&> {
  using iterator_type = typename Range::iterator;
};

template <typename Range>
struct container_traits<Range&&> {
  using iterator_type = typename Range::iterator;
};
}  // namespace dsac