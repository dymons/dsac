#pragma once

#include <vector>
#include <unordered_map>

namespace algo::graph {
class Digraph final {
 public:
  struct Node;
  using Successors = std::vector<Node>;
  using AdjacencyList = std::vector<Successors>;

  explicit Digraph(int size_graph) : nodes_(size_graph) {
  }

  void AddEdge(Node from, Node to) {
    nodes_[from.id].push_back(to);
  }

  const Successors& GetSuccessors(const Node node) {
    return nodes_[node.id];
  }

  struct Node final {
    int id{};
    bool operator==(const Node other) const noexcept {
      return id == other.id;
    }
  };

 private:
  AdjacencyList nodes_;
};
}  // namespace algo::graph

namespace std {
template <>
struct hash<algo::graph::Digraph::Node> {
  [[gnu::always_inline]] std::size_t operator()(
      algo::graph::Digraph::Node node) const noexcept {
    return node.id;
  }
};
}  // namespace std