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
  SECTION("Parse a digraph with one node definition") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse a digraph with multiple node definition") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a; b; c;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse a digraph with one edge") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc {
        a -> b;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kEmptyDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse a digraph with multiple edges") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a -> b -> c;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse a digraph with node definitions and edge definitions without attributes") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a; b; c; d; e; f; g;
        a -> b -> c;
        d -> e -> f;
        a -> g;
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
  SECTION("Parse a digraph with one node definition and attributes") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a [latitude=59.940583223694446 longitude=30.314237045788026 city=saint-petersburg];
        b [latitude=55.7532891830182 longitude=37.62042002899189 city=moscow];
      }
    )graph";

    dsac::graphviz_syntax_analyzer graphviz_syntax(kDigraph);
    REQUIRE_NOTHROW(graphviz_syntax.parse());
  }
}