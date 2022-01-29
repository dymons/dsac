#pragma once

#include <stdexcept>
#include <vector>

namespace algo::graph {
class DisjointSetException : public std::logic_error {
 public:
  using std::logic_error::logic_error;
};

class DisjointSetOutOfRange : public DisjointSetException {
 public:
  DisjointSetOutOfRange() : DisjointSetException("Out of range disjoint set") {
  }
};

//////////////////////////////////////////////////////////////////////

class DisjointSet final {
  std::vector<std::size_t> parent_vertex_;

 public:
  explicit DisjointSet(std::size_t size_set);

  void Union(std::size_t vertex1, std::size_t vertex2);
  std::size_t Find(std::size_t vertex);
  bool IsConnected(std::size_t vertex1, std::size_t vertex2);
};
}  // namespace algo::graph