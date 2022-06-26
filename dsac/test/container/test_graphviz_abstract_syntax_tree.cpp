#include <catch2/catch.hpp>

#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

#include <fmt/format.h>

using namespace std::literals;

TEST_CASE("Checking visit of abstract syntax tree", "[graphviz][ast]") {
  SECTION("Visit an empty digraph") {
    constexpr std::string_view kGraphName       = "abc"sv;
    constexpr char const*      kEmptyDefinition = R"graph(
      digraph {} {{}}
    )graph";

    const std::string              digraph = fmt::format(kEmptyDefinition, kGraphName);
    dsac::graphviz_syntax_analyzer graphviz_syntax(digraph);
    dsac::ast_base_ref             graph = graphviz_syntax.parse();

    bool is_invoked = false;
    auto visitor    = [&](dsac::ast_direct_graph_node const& graph) {
      REQUIRE(graph.get_graph_name() == kGraphName);
      return (is_invoked = true, nullptr);
    };

    dsac::visit(visitor, graph);
    REQUIRE(is_invoked);
  }
  SECTION("Visit a digraph with one statement") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a -> b;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    dsac::ast_base_ref             graph = graphviz_syntax.parse();

    // clang-format off
    dsac::ast_base_ref statements = dsac::visit([&](dsac::ast_direct_graph_node const& graph) {
      return graph.get_statements();
    }, graph);
    // clang-format on

    REQUIRE(statements);

    dsac::visit(
        [](dsac::ast_statements_node const& statements) {
          REQUIRE(std::distance(statements.begin(), statements.end()) == 1);
          for (dsac::ast_base_ref const& statement : statements) {
            dsac::visit(
                [](dsac::ast_direct_edge_node const& edge) {
                  auto* from_cast = dsac::unsafe_to<dsac::ast_node_node>(edge.get_from_node());
                  auto* to_cast   = dsac::unsafe_to<dsac::ast_node_node>(edge.get_to_node());

                  REQUIRE(from_cast->get_name() == "a"sv);
                  REQUIRE(to_cast->get_name() == "b"sv);

                  return nullptr;
                },
                statement);
          }
          return nullptr;
        },
        statements);
  }
}
