#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

namespace dsac::graph {
struct Node final {
  int id{};
  [[gnu::always_inline]] bool operator==(const Node other) const noexcept {
    return id == other.id;
  }
};
}  // namespace dsac::graph

namespace std {
template <>
struct hash<dsac::graph::Node> {
  [[gnu::always_inline]] std::size_t operator()(dsac::graph::Node node) const noexcept {
    return node.id;
  }
};
}  // namespace std

namespace dsac::graph {
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
}  // namespace dsac::graph
