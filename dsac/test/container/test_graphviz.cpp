#include <catch2/catch.hpp>

#include <dsac/container/graph/directed_graph.hpp>
#include <dsac/container/graph/read_graphviz.hpp>

using namespace std::literals;

struct node final {
  std::string                      key{};
  dsac::dynamic_array<std::string> attributes{};

  static node make(std::string key) {
    return {.key = std::move(key)};
  }
};

struct edge final {
  std::string                      key{};
  dsac::dynamic_array<std::string> attributes{};

  static edge make(std::string key) {
    return {.key = std::move(key)};
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

TEST_CASE("Building directed graph from DOT format", "[graph][graphviz]") {
  constexpr char const* kDigraph = R"graph(
    digraph abc {
      a -> b -> c;
    }
  )graph";

  dsac::directed_graph<node, edge> graph = dsac::read_graphviz<node, edge>(kDigraph);

  auto* node_a = graph.get_node("a").base();
  auto* node_b = graph.get_node("b").base();
  auto* node_c = graph.get_node("c").base();

  REQUIRE(node_a != nullptr);
  REQUIRE(node_b != nullptr);
  REQUIRE(node_c != nullptr);

  REQUIRE(node_a->edges.size() == 1);
  REQUIRE(node_a->edges[0]->from == node_a);
  REQUIRE(node_a->edges[0]->to == node_b);
  REQUIRE(node_b->edges.size() == 1);
  REQUIRE(node_b->edges[0]->from == node_b);
  REQUIRE(node_b->edges[0]->to == node_c);
  REQUIRE(node_c->edges.empty());
}