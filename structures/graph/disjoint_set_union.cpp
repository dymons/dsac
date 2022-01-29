#include <structures/graph/disjoint_set_union.hpp>

#include <utility>
#include <numeric>

namespace algo::graph {
DisjointSet::DisjointSet(std::size_t size_set) : parents_(size_set) {
  std::iota(parents_.begin(), parents_.end(), 0);
}

void DisjointSet::Union(std::size_t x, std::size_t y) {
  while (parents_[x] != parents_[y]) {
    if (parents_[x] < parents_[y]) {
      std::size_t const oldest_parent_x = std::exchange(parents_[x], parents_[y]);
      if (x == oldest_parent_x) {
        return;
      }
      x = oldest_parent_x;
    } else {
      std::size_t const oldest_parent_y = std::exchange(parents_[y], parents_[x]);
      if (y == oldest_parent_y) {
        return;
      }
      y = oldest_parent_y;
    }
  }
}
std::size_t DisjointSet::Find(std::size_t i) {
  std::size_t start = i;
  std::size_t root = parents_[i];
  while (root != i) {
    i = root;
    root = parents_[i];
  }

  i = start;
  std::size_t parent = parents_[i];
  while (parent != root) {
    parents_[i] = root;
    i = parent;
    parent = parents_[i];
  }

  return root;
}
}  // namespace algo::graph