#include <catch2/catch.hpp>

#include <breadth_first_search.hpp>

TEST_CASE("Поиск в ширину на графе", "[breadth_first_search]") {
  using namespace algo::graph;
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

  Digraph graph(edges.size());
  for (const Edge& edge : edges) {
    graph.AddEdge({edge.first}, {edge.second});
  }

  REQUIRE(BreadthFirstSearch::PathExist(graph, {0}, {2}));
  REQUIRE(BreadthFirstSearch::PathExist(graph, {1}, {2}));
  REQUIRE(BreadthFirstSearch::PathExist(graph, {2}, {2}));
  REQUIRE_FALSE(BreadthFirstSearch::PathExist(graph, {2}, {0}));
  REQUIRE_FALSE(BreadthFirstSearch::PathExist(graph, {2}, {3}));

  const BreadthFirstSearch::Path expected{{1}, {3}, {2}};
  REQUIRE(BreadthFirstSearch::Search(graph, {1}, {2}) == expected);
}