#include <catch2/catch.hpp>

#include <graph.hpp>

TEST_CASE("Проверка корректности работы неориентированного графа",
          "[undirected_graph]") {
  algo::graph::UndirectedGraph<int> graph(4);
  graph.Add({0}, {1});
  graph.Add({0}, {2});
  graph.Add({1}, {3});
  graph.Add({3}, {2});
  REQUIRE(graph.GetSuccessors({0}).size() == 2);
  REQUIRE(graph.GetSuccessors({1}).size() == 1);
  REQUIRE(graph.GetSuccessors({2}).empty());
  REQUIRE(graph.GetSuccessors({3}).size() == 1);
}