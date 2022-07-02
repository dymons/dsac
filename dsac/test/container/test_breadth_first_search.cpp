#include <catch2/catch.hpp>

#include <dsac/container/graph/breadth_first_search.hpp>
#include <dsac/container/graph/directed_graph.hpp>
#include <dsac/container/graph/read_graphviz.hpp>

namespace {

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

template <typename G, typename... Args>
auto make_expected_shortest_path(G const& g, Args&&... args) {
  return std::make_optional(std::vector{(g.get_node(std::forward<Args>(args)))...});
}

}  // namespace

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

TEST_CASE("Testcases for checking finding shortest path using Breadth-first search", "[graph][bfs]") {
  constexpr char const* kDigraph = R"graph(
    digraph abc {
      a -> b -> c;
    }
  )graph";

  const auto graph                  = dsac::read_graphviz<node, edge>(kDigraph);
  const auto shortest_path          = dsac::bfs(graph, "a", "c");
  const auto expected_shortest_path = make_expected_shortest_path(graph, "a", "b", "c");
  REQUIRE(shortest_path == expected_shortest_path);
}
