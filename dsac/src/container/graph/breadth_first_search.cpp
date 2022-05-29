#include <dsac/container/graph/breadth_first_search.hpp>

#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace dsac::graph::bfs {
bool IsPathExist(Graph& graph, const Node from, const Node to) {
  std::queue<Node> processing;
  processing.push(from);

  std::unordered_set<Node> visited;
  visited.emplace(from);

  while (!processing.empty()) {
    if (const Node considered = processing.front(); considered == to)
        [[unlikely]] {
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

Path ShortestPath(Graph& graph, const Node from, const Node to) {
  std::queue<Node> processing;
  processing.push(from);

  std::unordered_map<Node, Node> possible_actions;
  possible_actions[from] = from;

  std::unordered_set<Node> visited;
  visited.emplace(from);

  while (!processing.empty()) {
    if (const Node considered = processing.front(); considered == to)
        [[unlikely]] {
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
}  // namespace dsac::graph::bfs