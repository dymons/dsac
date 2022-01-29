#pragma once

#include <vector>

namespace algo::graph {
class DisjointSet final {
  std::vector<std::size_t> parents_;

 public:
  explicit DisjointSet(std::size_t size_set);

  void Union(std::size_t x, std::size_t y);
  std::size_t Find(std::size_t i);
};
}  // namespace algo::graph