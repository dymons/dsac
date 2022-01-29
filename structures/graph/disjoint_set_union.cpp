#include <structures/graph/disjoint_set_union.hpp>

#include <utility>
#include <numeric>

namespace algo::graph {
DisjointSet::DisjointSet(std::size_t size_set) : parent_vertex_(size_set) {
  std::iota(parent_vertex_.begin(), parent_vertex_.end(), 0);
}

void DisjointSet::Union(std::size_t vertex1, std::size_t vertex2) {
  if (!IsConnected(vertex1, vertex2)) {
    std::size_t const parent1 = Find(vertex1);
    std::size_t const parent2 = Find(vertex2);
    for (std::size_t i{}; i < parent_vertex_.size(); ++i) {
      if (parent_vertex_[i] == parent2) {
        parent_vertex_[i] = parent1;
      }
    }
  }
}

std::size_t DisjointSet::Find(std::size_t const vertex) const {
  if (vertex >= parent_vertex_.size()) {
    throw DisjointSetOutOfRange{};
  }
  return parent_vertex_[vertex];
}

bool DisjointSet::IsConnected(std::size_t vertex1, std::size_t vertex2) const {
  return Find(vertex1) == Find(vertex2);
}
}  // namespace algo::graph