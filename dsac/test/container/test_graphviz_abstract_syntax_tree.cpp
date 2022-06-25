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
    const std::string          digraph          = fmt::format(kEmptyDefinition, kGraphName);

    dsac::graphviz_syntax_analyzer graphviz_syntax(digraph);
    dsac::ast_base_ref             graph = graphviz_syntax.parse();

    const auto visitor = [&graph_name = kGraphName](dsac::ast_direct_graph_node* graph) -> dsac::ast_base* {
      REQUIRE(graph != nullptr);
      REQUIRE(graph->get_graph_name() == graph_name);
      return nullptr;
    };

    dsac::visit(visitor, graph.get());
  }
}
