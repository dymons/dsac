#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

namespace algo::graph {
struct Node final {
  int id{};
  [[gnu::always_inline]] bool operator==(const Node other) const noexcept {
    return id == other.id;
  }
};
}  // namespace algo::graph

namespace std {
template <>
struct hash<algo::graph::Node> {
  [[gnu::always_inline]] std::size_t operator()(algo::graph::Node node) const noexcept {
    return node.id;
  }
};
}  // namespace std

namespace algo::graph {
class Digraph final {
 public:
  using Successors = std::vector<Node>;
  using AdjacencyList = std::unordered_map<Node, Successors>;
  using Visitor = std::function<void(Node)>;

  void AddEdge(Node from, Node to) {
    graph_[from].push_back(to);
  }

  Successors const& GetSuccessors(const Node node) {
    return graph_[node];
  }

  void Visit(Visitor visitor) const {
    for (const auto& [node, successors] : graph_) {
      visitor(node);
    }
  }

 private:
  AdjacencyList graph_;
};
}  // namespace algo::graph
