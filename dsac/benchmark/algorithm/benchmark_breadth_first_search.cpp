#include <benchmark/benchmark.h>

#include <dsac/container/graph/directed_graph.hpp>
#include <dsac/container/graph/read_graphviz.hpp>
#include "dsac/algorithm/breadth_first_search.hpp"

#include <fmt/format.h>

namespace {

struct node final {
  std::string key{};
  static node make(std::string key) {
    return {.key = std::move(key)};
  }
};

struct edge final {
  std::string key{};
  static edge make(std::string key) {
    return {.key = std::move(key)};
  }
};

constexpr char const* kGoPackageImports = R"graph(
  digraph packages {
    n0 -> n1;
    n0 -> n2;
    n0 -> n3;
    n0 -> n4;
    n0 -> n5;
    n0 -> n6;
    n0 -> n7;
    n0 -> n8;
    n0 -> n9;
    n1 -> n10;
    n1 -> n2;
    n1 -> n8;
    n1 -> n9;
    n1 -> n11;
    n2 -> n11;
    n2 -> n7;
    n3 -> n4;
    n3 -> n5;
    n3 -> n6;
    n3 -> n8;
    n3 -> n9;
    n4 -> n12;
    n5 -> n10;
    n5 -> n13;
    n5 -> n9;
    n5 -> n11;
    n5 -> n14;
    n6 -> n2;
    n6 -> n7;
    n6 -> n15;
    n6 -> n11;
    n6 -> n10;
    n6 -> n8;
    n6 -> n9;
    n7 -> n16;
    n7 -> n17;
    n7 -> n18;
    n7 -> n15;
    n10 -> n19;
    n10 -> n15;
    n11 -> n12;
    n12 -> n17;
    n12 -> n15;
    n13 -> n15;
    n13 -> n19;
    n13 -> n14;
    n14 -> n15;
    n16 -> n15;
    n18 -> n15;
  }
)graph";

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

static void bm_breadth_first_search(benchmark::State& state) {
  const auto graph = dsac::read_graphviz<node, edge>(kGoPackageImports);
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    dsac::bfs(graph, fmt::format("n{}", state.range(0)), fmt::format("n{}", state.range(1)));
  }
}

BENCHMARK(bm_breadth_first_search)  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    ->Args({0, 4})
    ->Args({0, 8})
    ->Args({0, 16});
