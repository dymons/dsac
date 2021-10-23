#pragma once

#include <directed_graph.hpp>

#include <stack>
#include <unordered_set>
#include <unordered_map>

namespace algo::graph {
class DepthFirstSearch {
 public:
  using Graph = Digraph;
  using Node = typename Graph::Node;
  using Path = std::deque<Node>;

  static bool PathExist(Graph& graph, const Node from, const Node to) {
    std::stack<Node> processing;
    processing.push(from);

    std::unordered_set<Node> visited;
    visited.emplace(from);

    while (!processing.empty()) {
      if (const Node considered = processing.top(); considered == to) {
        return true;
      } else {
        processing.pop();
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
            processing.push(successor);
          }
        }
      }
    }

    return false;
  }

  static Path Search(Graph& graph, const Node from, const Node to) {
    std::stack<Node> processing;
    processing.push(from);

    std::unordered_map<Node, Node> possible_actions;
    possible_actions[from] = from;

    std::unordered_set<Node> visited;
    visited.emplace(from);

    while (!processing.empty()) {
      if (const Node considered = processing.top(); considered == to) {
        break;
      } else {
        processing.pop();
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
            possible_actions[successor] = considered;
            processing.push(successor);
          }
        }
      }
    }

    Path path{to};
    for (Node latest = path.front(); latest != from; latest = path.front()) {
      if (!possible_actions.contains(latest)) {
        break;
      }
      path.push_front(possible_actions[latest]);
    }

    return path.front() == from ? path : (path.clear(), path);
  }
};
}  // namespace algo::graph