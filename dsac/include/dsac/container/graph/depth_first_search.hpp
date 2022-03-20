#pragma once

#include "directed_graph.hpp"
#include <deque>

namespace dsac::graph::dfs {
/// \brief      Алгоритм поиска реализован для ориентированного-невзвешенного
///             графа
using Graph = Digraph;
using Path = std::deque<Node>;

/// \brief      Проверка существования маршрута в графе
[[nodiscard]] bool IsPathExist(Graph& graph, const Node from, const Node to);

/// \brief      Поиск короткого маршрута движения
[[nodiscard]] Path ShortestPath(Graph& graph, const Node from, const Node to);
}  // namespace dsac::graph::dfs