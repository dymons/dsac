#pragma once

#include <directed_graph.hpp>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <deque>

namespace algo::graph {
class BreadthFirstSearch {
 public:
  using Graph = Digraph;
  using Node = typename Graph::Node;
  using Path = std::deque<Node>;

  static bool PathExist(Graph& graph, const Node from, const Node to) {
    std::queue<Node> processing;
    processing.push(from);

    std::unordered_set<Node> visited;
    while (!processing.empty()) {
      if (const Node considered = processing.front(); considered == to) {
        return true;
      } else {
        processing.pop();
        visited.emplace(considered);
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (!visited.contains(successor)) {
            processing.push(successor);
          }
        }
      }
    }

    return false;
  }

  static Path Search(Graph& graph, const Node from, const Node to) {
    std::queue<Node> processing;
    processing.push(from);

    std::unordered_map<Node, Node> possible_actions;
    possible_actions[from] = from;

    while (!processing.empty()) {
      if (const Node considered = processing.front(); considered == to) {
        break;
      } else {
        processing.pop();
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (!possible_actions.contains(successor)) {
            processing.push(successor);
            possible_actions[successor] = considered;
          }
        }
      }
    }

    Path path{to};
    for (Node latest = path.front(); latest != from; latest = path.front()) {
      path.push_front(possible_actions[latest]);
    }

    return path;
  }
};
}  // namespace algo::graph