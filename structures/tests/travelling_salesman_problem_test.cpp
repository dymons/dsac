#include <catch2/catch.hpp>
#include <utility>

#include <structures/graph/travelling_salesman_problem.hpp>

TEST_CASE("Задача коммивояжёра", "[travelling_salesman_problem]") {
  using namespace algo::graph;

  constexpr int size_graph = 4;
  tsp::CompleteGraph graph(size_graph);

  graph.AddEdge({0}, {1}, 4);
  graph.AddEdge({0}, {2}, 1);
  graph.AddEdge({0}, {3}, 9);

  graph.AddEdge({1}, {0}, 3);
  graph.AddEdge({1}, {2}, 6);
  graph.AddEdge({1}, {3}, 11);

  graph.AddEdge({2}, {0}, 4);
  graph.AddEdge({2}, {1}, 1);
  graph.AddEdge({2}, {3}, 2);

  graph.AddEdge({3}, {0}, 6);
  graph.AddEdge({3}, {1}, 5);
  graph.AddEdge({3}, {2}, -4);

  const tsp::TravellingSalesmanResult expected{.tour = {{0}, {3}, {2}, {1}, {0}},
                                               .min_cost = 9};
  const tsp::TravellingSalesmanResult solved = tsp::Solve(graph, {0});
  REQUIRE(solved.tour == expected.tour);
  REQUIRE(solved.min_cost == expected.min_cost);
}