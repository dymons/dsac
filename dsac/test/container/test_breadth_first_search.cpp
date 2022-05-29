#include <catch2/catch.hpp>

#include <dsac/container/graph/breadth_first_search.hpp>

TEST_CASE("Поиск в ширину на графе", "[breadth_first_search]") {
  using namespace dsac::graph;
  using Edge  = std::pair<int, int>;
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
    REQUIRE(bfs::IsPathExist(graph, {0}, {8}));
    REQUIRE(bfs::IsPathExist(graph, {3}, {6}));
    REQUIRE(bfs::IsPathExist(graph, {7}, {8}));
    REQUIRE(bfs::IsPathExist(graph, {8}, {7}));
    REQUIRE_FALSE(bfs::IsPathExist(graph, {10}, {1}));
    REQUIRE_FALSE(bfs::IsPathExist(graph, {8}, {3}));
  }
  SECTION("Поиск маршрута между двумя узлами в графе") {
    const bfs::Path expected{{3}, {4}, {9}, {8}, {7}};
    REQUIRE(bfs::ShortestPath(graph, {3}, {7}) == expected);
    REQUIRE(bfs::ShortestPath(graph, {7}, {1}).empty());
    REQUIRE(bfs::ShortestPath(graph, {4}, {0}).empty());
  }
}