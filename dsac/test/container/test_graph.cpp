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
  std::string                      key{};
  dsac::dynamic_array<std::string> attributes{};

  [[gnu::always_inline]] inline static node make_empty() {
    return {};
  }
};

struct edge final {
  std::string                      key{};
  dsac::dynamic_array<std::string> attributes{};

  [[gnu::always_inline]] inline static edge make_empty() {
    return {};
  }
};

template <>
struct dsac::directed_graph_semantic<node> {
  using key_type = std::string;
  [[gnu::always_inline]] inline static auto get_key(node const& node) -> key_type {
    return node.key;
  };
};

template <>
struct dsac::directed_graph_semantic<edge> {
  using key_type = std::string;
  [[gnu::always_inline]] inline static auto get_key(edge const& edge) -> key_type {
    return edge.key;
  };
};

TEST_CASE("Testcases for checking building direct graph", "[graph][direct]") {
  GIVEN("An empty directed graph") {
    dsac::directed_graph<node, edge> graph;
    WHEN("Store a default node in the directed graph") {
      auto const [unique_node, added] = graph.insert_node(node::make_empty());
      THEN("The stored node was successfully added") {
        REQUIRE(added);
      }
      THEN("The node has an empty key and attributes") {
        REQUIRE((*unique_node).key.empty());
        REQUIRE((*unique_node).attributes.empty());
      }
      WHEN("Store a default node in the directed graph again") {
        auto const [source_node, added_again] = graph.insert_node(node::make_empty());
        THEN("The stored node was not successfully added") {
          REQUIRE_FALSE(added_again);
        }
        THEN("Was returned the same iterator to the default node") {
          REQUIRE(source_node == unique_node);
        }
      }
    }
    WHEN("Store a default edge in the directed graph") {
      auto spb                        = graph.insert_node(node{.key = "Saint-Petersburg"});
      auto moscow                     = graph.insert_node(node{.key = "Moscow"});
      const auto [unique_edge, added] = graph.insert_direct_edge(spb.first, moscow.first, edge::make_empty());
      THEN("The stored edge was successfully added") {
        REQUIRE(added);
      }
      THEN("The node has an empty key and attributes") {
        REQUIRE((*unique_edge).key.empty());
        REQUIRE((*unique_edge).attributes.empty());
      }
      WHEN("Store a default node in the directed graph again") {
        const auto [source_edge, added_again] = graph.insert_direct_edge(spb.first, moscow.first, edge::make_empty());
        THEN("The stored node was not successfully added") {
          REQUIRE_FALSE(added_again);
        }
        THEN("Was returned the same iterator to the default node") {
          REQUIRE(source_edge == unique_edge);
        }
      }
    }
    WHEN("Create a cycle for one node") {
      auto const node = graph.insert_node(node::make_empty());
      REQUIRE_THROWS_AS(graph.insert_direct_edge(node.first, node.first, edge::make_empty()), dsac::loops_not_supported);
    }
  }
}
