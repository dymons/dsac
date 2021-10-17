#pragma once

#include <vector>
#include <unordered_map>

namespace algo::graph {
class OrientedGraph final {
 public:
  struct Node;
  using Successors = std::vector<Node>;
  using AdjacencyList = std::vector<Successors>;

  explicit OrientedGraph(int size_graph) : nodes_(size_graph) {
  }

  void Add(Node from, Node to) {
    nodes_[from.id].push_back(to);
  }

  const Successors& GetSuccessors(const Node node) {
    return nodes_[node.id];
  }

  struct Node final {
    int id{};
    bool operator==(const Node other) const {
      return id == other.id;
    }
  };

 private:
  AdjacencyList nodes_;
};
}  // namespace algo::graph