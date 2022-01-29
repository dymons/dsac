#include <structures/graph/disjoint_set_union.hpp>

#include <utility>
#include <numeric>

namespace algo::graph {
DisjointSet::DisjointSet(std::size_t size_set) : parent_vertex_(size_set) {
  std::iota(parent_vertex_.begin(), parent_vertex_.end(), 0);
}

void DisjointSet::Union(std::size_t vertex1, std::size_t vertex2) {
  while (!IsConnected(vertex1, vertex2)) {
    if (parent_vertex_[vertex1] >= parent_vertex_[vertex2]) {
      std::swap(vertex1, vertex2);
    }

    std::size_t parent = std::exchange(parent_vertex_[vertex2], parent_vertex_[vertex1]);
    if (vertex2 == parent) {
      return;
    }
    vertex2 = parent;
  }
}

std::size_t DisjointSet::Find(std::size_t const vertex) {
  if (vertex >= parent_vertex_.size()) {
    throw DisjointSetOutOfRange{};
  }
  return parent_vertex_[vertex];
}

bool DisjointSet::IsConnected(std::size_t vertex1, std::size_t vertex2) {
  return Find(vertex1) == Find(vertex2);
}
}  // namespace algo::graph