#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

namespace algo::graph {
class Digraph final {
 public:
  struct Node;
  using Successors = std::vector<Node>;
  using AdjacencyList = std::vector<Successors>;
  using Visitor = std::function<void(Node)>;

  explicit Digraph(int size_graph) : nodes_(size_graph) {
  }

  void AddEdge(Node from, Node to) {
    nodes_[from.id].push_back(to);
  }

  const Successors& GetSuccessors(const Node node) {
    return nodes_[node.id];
  }

  [[nodiscard]] std::size_t GetNumNodes() const noexcept {
    return nodes_.size();
  }

  void ForEach(Visitor visitor) {
    for (int i = 0; i < nodes_.size(); ++i) {
      visitor({i});
    }
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