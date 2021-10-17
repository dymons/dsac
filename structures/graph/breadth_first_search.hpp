#pragma once

#include <graph.hpp>
#include <queue>
#include <unordered_set>

namespace algo::graph {
class BreadthFirstSearch {
 public:
  using Graph = OrientedGraph;
  using Node = typename Graph::Node;
  static bool PathExist(Graph& graph, Node from, Node to) {
    std::queue<Node> processing;
    processing.push(from);

    std::unordered_set<Node> visited;
    bool found = false;
    while (!processing.empty()) {
      const Node current = processing.front();
      processing.pop();

      if (visited.contains(current)) {
        continue;
      } else {
        visited.emplace(current);
      }

      if (current == to) {
        found = true;
        break;
      }

      for (Node successor : graph.GetSuccessors(current)) {
        processing.push(successor);
      }
    }

    return found;
  }
};

}  // namespace algo::graph