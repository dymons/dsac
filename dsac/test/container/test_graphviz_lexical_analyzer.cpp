#include <catch2/catch.hpp>

#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

using namespace std::literals;

TEST_CASE("Checking the analytical analysis of the graphviz for recognition of tokens", "[graphviz][lexical]") {
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