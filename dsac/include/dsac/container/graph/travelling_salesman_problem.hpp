#pragma once

#include <vector>

namespace dsac::legacy_graph::tsp {
struct Node final {
  int                         id{};
  [[gnu::always_inline]] bool operator==(const Node other) const noexcept {
    return id == other.id;
  }
};

class CompleteGraph final {
public:
  using Cost            = int;
  using AdjacencyRow    = std::vector<Cost>;
  using AdjacencyMatrix = std::vector<AdjacencyRow>;
  explicit CompleteGraph(int n)
    : graph_(n, std::vector<Cost>(n, 0)) {
  }

  void AddEdge(Node from, Node to, Cost cost) {
    graph_[from.id][to.id] = cost;
  }

  [[nodiscard]] Cost GetCost(Node from, Node to) const {
    return graph_[from.id][to.id];
  }

  [[nodiscard]] int GetMatrixSize() const noexcept {
    return graph_.size();
  }

private:
  AdjacencyMatrix graph_;
};

struct TravellingSalesmanResult {
  std::vector<Node> tour;
  int               min_cost{};
};

[[nodiscard]] TravellingSalesmanResult Solve(CompleteGraph& graph, Node start);
}  // namespace dsac::legacy_graph::tsp