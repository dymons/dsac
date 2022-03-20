#pragma once

namespace dsac {
template <typename Range>
struct container_traits {
  using iterator_type = typename Range::iterator;
};
}  // namespace dsac