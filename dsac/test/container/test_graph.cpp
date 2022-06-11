#include <catch2/catch.hpp>
#include <utility>

#include <dsac/container/graph/directed_graph.hpp>

TEST_CASE("Проверка корректности работы неориентированного графа", "[undirected_graph]") {
  using Edge  = std::pair<int, int>;
  using Edges = std::vector<Edge>;
  // clang-format off
  const Edges edges {
    {0, 1},
    {0, 2},
    {1, 3},
    {3, 2},
  };
  // clang-format on

  dsac::legacy_graph::Digraph graph;
  for (const Edge& edge : edges) {
    graph.AddEdge({edge.first}, {edge.second});
  }

  REQUIRE(graph.GetSuccessors({0}).size() == 2);
  REQUIRE(graph.GetSuccessors({1}).size() == 1);
  REQUIRE(graph.GetSuccessors({2}).empty());
  REQUIRE(graph.GetSuccessors({3}).size() == 1);
}

struct node final {
  std::string                      key;
  dsac::dynamic_array<std::string> attributes;
};

struct edge final {
  std::string                      key;
  dsac::dynamic_array<std::string> attributes;
};

template <>
struct dsac::detail::nth_key<node> {
  inline static auto get(node const& node) -> std::string_view {
    return node.key;
  };
};

template <>
struct dsac::detail::nth_key<edge> {
  inline static auto get(edge const& edge) -> std::string_view {
    return edge.key;
  };
};

TEST_CASE("Testcases for checking direct graph", "[graph][direct]") {
  dsac::directed_graph<node, edge> graph;
}
