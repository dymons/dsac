#include <dsac/container/graph/disjoint_set_union.hpp>

#include <numeric>
#include <utility>

namespace dsac::legacy_graph {
DisjointSet::DisjointSet(std::size_t size_set)
  : parent_vertex_(size_set) {
  std::iota(parent_vertex_.begin(), parent_vertex_.end(), 0);
}

void DisjointSet::Union(std::size_t vertex1, std::size_t vertex2) {
  if (!IsConnected(vertex1, vertex2)) {
    std::size_t const parent1 = Find(vertex1);
    std::size_t const parent2 = Find(vertex2);
    parent_vertex_[parent2]   = parent1;
  }
}

std::size_t DisjointSet::Find(std::size_t vertex) {
  if (vertex >= parent_vertex_.size()) {
    throw DisjointSetOutOfRange{};
  }

  // Step 1. Find the parent vertex
  std::size_t start_vertex  = vertex;
  std::size_t parent_vertex = parent_vertex_[vertex];
  while (parent_vertex != vertex) {
    vertex        = parent_vertex;
    parent_vertex = parent_vertex_[vertex];
  }

  // Step 2. Fill in the completed route with a new parent vertex
  vertex                        = start_vertex;
  std::size_t old_parent_vertex = parent_vertex_[vertex];
  while (old_parent_vertex != parent_vertex) {
    parent_vertex_[vertex] = parent_vertex;
    vertex                 = old_parent_vertex;
    old_parent_vertex      = parent_vertex_[vertex];
  }

  return parent_vertex;
}

bool DisjointSet::IsConnected(std::size_t vertex1, std::size_t vertex2) {
  return Find(vertex1) == Find(vertex2);
}
}  // namespace dsac::legacy_graph