#include <structures/graph/dijkstra_search.hpp>

#include <unordered_set>
#include <unordered_map>

namespace algo::graph::dijkstra {
namespace detail {
using Distance = int;

static auto InitDistances(Graph& graph) {
  std::unordered_map<Node, Distance> distances;
  graph.ForEach([&](Node node) {
    distances.emplace(node, std::numeric_limits<Distance>::max());
  });
  return distances;
}

static auto InitPriorityQueue() {
  struct ComparedNode {
    ComparedNode(Node node, Distance distance) : node(node), distance(distance) {
    }

    Node node;
    Distance distance;

    [[gnu::always_inline]] bool operator<(const ComparedNode& other) const noexcept {
      return distance < other.distance;
    }
  };

  return std::priority_queue<ComparedNode>{};
}
}  // namespace detail

bool IsPathExist(Graph& graph, const Node from, const Node to) {
  auto distances = detail::InitDistances(graph);
  distances[from] = 0;

  auto processing = detail::InitPriorityQueue();
  processing.emplace(from, 0);

  std::unordered_set<Node> visited;
  visited.emplace(from);

  while (!processing.empty()) {
    if (auto [considered, min_dist] = processing.top(); considered == to) [[unlikely]] {
      return true;
    } else {
      processing.pop();
      if (distances[considered] >= min_dist) [[likely]] {
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
            const detail::Distance new_dist = distances[considered] + 1;
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

Path ShortestPath(Graph& graph, const Node from, const Node to) {
  auto distances = detail::InitDistances(graph);
  distances[from] = 0;

  auto processing = detail::InitPriorityQueue();
  processing.emplace(from, 0);

  std::unordered_map<Node, Node> possible_actions;
  possible_actions[from] = from;

  std::unordered_set<Node> visited;
  visited.emplace(from);

  while (!processing.empty()) {
    if (auto [considered, min_dist] = processing.top(); considered == to) [[unlikely]] {
      break;
    } else {
      processing.pop();
      if (distances[considered] >= min_dist) [[likely]] {
        for (const Node successor : graph.GetSuccessors(considered)) {
          if (auto [it, not_exist] = visited.emplace(successor); not_exist) {
            const detail::Distance new_dist = distances[considered] + 1;
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
    if (!possible_actions.contains(latest)) [[unlikely]] {
      break;
    }
    path.push_front(possible_actions[latest]);
  }

  return path.front() == from ? path : (path.clear(), path);
}
}  // namespace algo::graph::dijkstra