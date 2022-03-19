#include "catch2/catch.hpp"

#include <structures/graph/depth_first_search.hpp>

TEST_CASE("Поиск в глубину на графе", "[depth_first_search]") {
  using namespace algo::graph;
  using Edge = std::pair<int, int>;
  using Edges = std::vector<Edge>;
  // clang-format off
  const Edges edges {
    {0, 1},
    {0, 2},
    {1, 3},
    {2, 3},
    {3, 4},
    {4, 5},
    {5, 6},
    {6, 4},
    {4, 9},
    {9, 8},
    {8, 7},
    {7, 4},
    {9, 10},
  };
  // clang-format on

  Digraph graph;
  for (const Edge& edge : edges) {
    graph.AddEdge({edge.first}, {edge.second});
  }

  SECTION("Проверка существования маршрута в графе между узлами") {
    REQUIRE(dfs::IsPathExist(graph, {0}, {8}));
    REQUIRE(dfs::IsPathExist(graph, {3}, {6}));
    REQUIRE(dfs::IsPathExist(graph, {7}, {8}));
    REQUIRE(dfs::IsPathExist(graph, {8}, {7}));
    REQUIRE_FALSE(dfs::IsPathExist(graph, {10}, {1}));
    REQUIRE_FALSE(dfs::IsPathExist(graph, {8}, {3}));
  }
  SECTION("Поиск маршрута между двумя узлами в графе") {
    const dfs::Path expected{{3}, {4}, {9}, {8}, {7}};
    REQUIRE(dfs::ShortestPath(graph, {3}, {7}) == expected);
    REQUIRE(dfs::ShortestPath(graph, {7}, {1}).empty());
    REQUIRE(dfs::ShortestPath(graph, {4}, {0}).empty());
  }
}