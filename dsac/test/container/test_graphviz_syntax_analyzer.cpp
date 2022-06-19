#include <catch2/catch.hpp>

#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

TEST_CASE("Checking the syntax analysis of the graphviz", "[graphviz][syntax]") {
  SECTION("Parse an empty digraph with correct syntax") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc {}
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse an empty digraph with incorrect type of graph") {
    constexpr char const* kEmptyDigraph = R"graph(
      my_graph abc {}
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_THROWS(graphviz_syntax.parse(), dsac::unexpected_syntax{});
  }
  SECTION("Parse an empty digraph with incorrect brackets for statement list") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc []
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_THROWS(graphviz_syntax.parse(), dsac::unexpected_syntax{});
  }
  SECTION("Parse an empty digraph with node definition") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc {
        a;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse an empty digraph with multiple node definition") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc {
        a; b; c;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
}