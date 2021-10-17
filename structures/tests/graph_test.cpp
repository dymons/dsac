#include <catch2/catch.hpp>
#include <utility>

#include <graph.hpp>

TEST_CASE("Проверка корректности работы неориентированного графа",
          "[undirected_graph]") {
  using Edge = std::pair<int, int>;
  using Edges = std::vector<Edge>;
  // clang-format off
  const Edges edges {
    {0, 1},
    {0, 2},
    {1, 3},
    {3, 2},
  };
  // clang-format on

  algo::graph::UndirectedGraph<int> graph(edges.size());
  for (const Edge& edge : edges) {
    graph.Add({edge.first}, {edge.second});
  }

  REQUIRE(graph.GetSuccessors({0}).size() == 2);
  REQUIRE(graph.GetSuccessors({1}).size() == 1);
  REQUIRE(graph.GetSuccessors({2}).empty());
  REQUIRE(graph.GetSuccessors({3}).size() == 1);
}