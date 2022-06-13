#include <catch2/catch.hpp>

#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

TEST_CASE("Checking the analytical analysis of the graphviz for recognition of tokens", "[graphviz][lexical]") {
  constexpr char const* kDigraph = R"graph(
    digraph abc {
      a -> b -> c;
      b -> d;
    }
  )graph";

  dsac::graphviz_lexical_analyzer tokenizer(kDigraph);
}