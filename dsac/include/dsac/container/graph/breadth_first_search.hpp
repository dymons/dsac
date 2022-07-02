#pragma once

#include <dsac/container/dynamic_array.hpp>
#include <dsac/container/graph/directed_graph.hpp>

#include <deque>
#include <optional>
#include <queue>
#include <unordered_set>

namespace dsac::legacy_graph::bfs {
/// \brief      Алгоритм поиска реализован для ориентированного-невзвешенного
///             графа
using Graph = Digraph;
using Path  = std::deque<Node>;

/// \brief      Проверка существования маршрута в графе
[[nodiscard]] bool IsPathExist(Graph& graph, const Node from, const Node to);

/// \brief      Поиск короткого маршрута движения
[[nodiscard]] Path ShortestPath(Graph& graph, const Node from, const Node to);
}  // namespace dsac::legacy_graph::bfs

namespace dsac {

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
std::optional<std::vector<typename directed_graph<N, E, A, B>::node_iterator>> bfs(
    directed_graph<N, E, A, B> const&                         graph,
    typename directed_graph<N, E, A, B>::node_key_type const& source,
    typename directed_graph<N, E, A, B>::node_key_type const& destination) {
  using node_iterator = typename directed_graph<N, E, A, B>::node_iterator;

  auto const source_node      = graph.get_node(source);
  auto const destination_node = graph.get_node(destination);

  std::queue<node_iterator> processing;
  processing.push(source_node);

  std::unordered_map<node_iterator, node_iterator> possible_actions;
  possible_actions[source_node] = source_node;

  std::unordered_set<node_iterator> visited;
  visited.emplace(source_node);

  bool found = false;
  while (!processing.empty()) {
    const node_iterator considered = processing.front();
    if (considered == destination_node) {
      found = true;
      break;
    }

    processing.pop();
    for (auto const& edge : considered->edges) {
      if (auto const [_, not_exist] = visited.emplace(edge->to); not_exist) {
        possible_actions[edge->to] = considered;
        processing.push(edge->to);
      }
    }
  }

  if (!found) {
    return std::nullopt;
  }

  std::vector<node_iterator> path{destination_node};
  for (node_iterator latest = path.front(); latest != source_node; latest = path.front()) {
    if (!possible_actions.contains(latest)) {
      break;
    }
    path.insert(path.begin(), possible_actions[latest]);
  }

  return std::move(path);
}

}  // namespace dsac