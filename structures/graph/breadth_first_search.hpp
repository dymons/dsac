#pragma once

#include <graph.hpp>
#include <queue>

namespace algo::graph {
class BreadthFirstSearch {
 public:
  using Graph = OrientedGraph;
  using Node = typename Graph::Node;
  static bool PathExist(Graph& graph, Node from, Node to) {
    std::queue<Node> visited;
    visited.push(from);

    bool found = false;
    while (!visited.empty()) {
      const Node current = visited.front();
      visited.pop();

      if (current == to) {
        found = true;
        break;
      }

      for (Node successor : graph.GetSuccessors(current)) {
        visited.push(successor);
      }
    }

    return found;
  }
};

}  // namespace algo::graph