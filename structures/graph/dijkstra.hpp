#pragma once

#include <directed_graph.hpp>

#include <queue>
#include <unordered_set>
#include <unordered_map>

namespace algo::graph {
class Dijkstra {
 public:
  using Graph = Digraph;
  using Node = typename Graph::Node;
  using Path = std::deque<Node>;

  static auto InitDistances_Internal(Graph& graph) {
    std::unordered_map<Node, int> distances;
    graph.ForEach([&](Node node) {
      distances.emplace(node, std::numeric_limits<int>::max());
    });
    return distances;
  }

  static auto InitPriorityQueue_Internal() {
    struct ComparedNode {
      ComparedNode(Node node, int distance) : node(node), distance(distance) {
      }

      Node node;
      int distance;

      [[nodiscard]] bool operator<(const ComparedNode& other) const noexcept {
        return distance < other.distance;
      }
    };

    return std::priority_queue<ComparedNode>{};
  }

  static bool PathExist(Graph& graph, const Node from, const Node to) {
    std::unordered_map<Node, int> distances = InitDistances_Internal(graph);
    distances[from] = 0;

    auto processing = InitPriorityQueue_Internal();
    processing.emplace(from, 0);

    std::unordered_set<Node> visited;
    visited.emplace(from);

    while (!processing.empty()) {
      if (auto [considered, min_dist] = processing.top(); considered == to) {
        return true;
      } else {
        processing.pop();
        if (distances[considered] >= min_dist) {
          for (const Node successor : graph.GetSuccessors(considered)) {
            if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
              const int new_dist = distances[considered] + 1;
              if (new_dist < distances[successor]) {
                distances[successor] = new_dist;
                processing.emplace(successor, new_dist);
              }
            }
          }
        }
      }
    }

    return false;
  }

  static Path Search(Graph& graph, const Node from, const Node to) {
    std::unordered_map<Node, int> distances = InitDistances_Internal(graph);
    distances[from] = 0;

    auto processing = InitPriorityQueue_Internal();
    processing.emplace(from, 0);

    std::unordered_map<Node, Node> possible_actions;
    possible_actions[from] = from;

    std::unordered_set<Node> visited;
    visited.emplace(from);

    while (!processing.empty()) {
      if (auto [considered, min_dist] = processing.top(); considered == to) {
        break;
      } else {
        processing.pop();
        if (distances[considered] >= min_dist) {
          for (const Node successor : graph.GetSuccessors(considered)) {
            if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
              const int new_dist = distances[considered] + 1;
              if (new_dist < distances[successor]) {
                distances[successor] = new_dist;
                processing.emplace(successor, new_dist);
                possible_actions[successor] = considered;
              }
            }
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