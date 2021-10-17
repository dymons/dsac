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
      if (const Node considered = processing.front(); considered == to) {
        return true;
      } else {
        processing.pop();
        if (visited.contains(considered)) {
          continue;
        } else {
          visited.emplace(considered);
        }

        for (const Node successor : graph.GetSuccessors(considered)) {
          processing.push(successor);
        }
      }
    }

    return false;
  }
};

}  // namespace algo::graph