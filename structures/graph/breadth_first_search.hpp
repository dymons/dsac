#pragma once

#include <graph.hpp>
#include <queue>
#include <unordered_set>

namespace algo::graph {
class BreadthFirstSearch {
 public:
  using Graph = OrientedGraph;
  using Node = typename Graph::Node;
  static bool PathExist(Graph& graph, const Node from, const Node to) {
    std::queue<Node> processing;
    processing.push(from);

    std::unordered_set<Node> visited;
    while (!processing.empty()) {
      const Node current = processing.front();
      processing.pop();

      if (visited.contains(current)) {
        continue;
      } else {
        visited.emplace(current);
      }

      if (current == to) {
        return true;
      }

      for (const Node successor : graph.GetSuccessors(current)) {
        processing.push(successor);
      }
    }

    return false;
  }
};

}  // namespace algo::graph