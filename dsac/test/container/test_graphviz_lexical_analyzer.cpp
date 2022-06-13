#include <catch2/catch.hpp>

#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

using namespace std::literals;

TEST_CASE("Checking the analytical analysis of the graphviz for recognition of tokens", "[graphviz][lexical]") {
  SECTION("Check empty digraph") {
    constexpr char const* kEmptyDigraph = R"graph(
      digraph abc {}
    )graph";

    dsac::graphviz_lexical_analyzer tokenizer(kEmptyDigraph);
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::keyword, "digraph"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "abc"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "{"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "}"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::eof, ""sv));
  }
  SECTION("Check empty graph") {
    constexpr char const* kEmptyGraph = R"graph(
      graph abc {}
    )graph";

    dsac::graphviz_lexical_analyzer tokenizer(kEmptyGraph);
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::keyword, "graph"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "abc"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "{"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "}"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::eof, ""sv));
  }
  SECTION("Check digraph with one statement") {
    constexpr char const* kDigraph = R"graph(
      digraph abc {
        a -> b -> c;
      }
    )graph";

    dsac::graphviz_lexical_analyzer tokenizer(kDigraph);
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::keyword, "digraph"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "abc"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "{"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "a"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::operator_, "->"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "b"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::operator_, "->"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "c"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, ";"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "}"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::eof, ""sv));
  }
  SECTION("Check graph with one statement") {
    constexpr char const* kGraph = R"graph(
      graph abc {
        a -- b -- c;
      }
    )graph";

    dsac::graphviz_lexical_analyzer tokenizer(kGraph);
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::keyword, "graph"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "abc"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "{"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "a"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::operator_, "--"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "b"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::operator_, "--"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::identifier, "c"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, ";"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::punctuator, "}"sv));
    REQUIRE(tokenizer.get_next_token() == std::make_pair(dsac::token::eof, ""sv));
  }
}