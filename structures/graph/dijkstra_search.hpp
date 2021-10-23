#pragma once

#include <structures/graph/directed_graph.hpp>
#include <queue>

namespace algo::graph::dijkstra {
/// \brief      Алгоритм поиска реализован для ориентированного-невзвешенного
///             графа
using Graph = Digraph;
using Path = std::deque<Node>;

/// \brief      Проверка существования маршрута в графе
[[nodiscard]] bool IsPathExist(Graph& graph, const Node from, const Node to);

/// \brief      Поиск короткого маршрута движения
[[nodiscard]] Path ShortestPath(Graph& graph, const Node from, const Node to);
}  // namespace algo::graph::dijkstra