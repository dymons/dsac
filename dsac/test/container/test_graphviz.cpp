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
      a -> b;
      b -> c;
    }
  )graph";

  dsac::directed_graph<node, edge> graph = dsac::read_graphviz<node, edge>(kDigraph);
  REQUIRE(graph.get_node("a").base() != nullptr);
  REQUIRE(graph.get_node("b").base() != nullptr);
  REQUIRE(graph.get_node("c").base() != nullptr);
}