#pragma once

#include <vector>

namespace algo::graph {
template <typename T>
class UndirectedGraph final {
 public:
  struct GraphNode;
  using Successors = std::vector<GraphNode>;
  using AdjacencyList = std::vector<Successors>;

  explicit UndirectedGraph(int size_graph) : nodes_(size_graph) {
  }

  void Add(GraphNode from, GraphNode to) {
    nodes_[from.id].push_back(to);
  }

  const Successors& GetSuccessors(GraphNode node) const {
    return nodes_[node.id];
  }

  struct GraphNode final {
    int id{};
    T data{};
  };

 private:
  AdjacencyList nodes_;
};
}  // namespace algo::graph