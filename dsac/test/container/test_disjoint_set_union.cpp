#include <catch2/catch.hpp>

#include <dsac/container/graph/disjoint_set_union.hpp>

TEST_CASE("Структура данных Disjoint set Union", "[disjoint]") {
  using namespace dsac::legacy_graph;

  constexpr std::size_t kSizeDisjointSet = 10U;
  DisjointSet           disjoint_set(kSizeDisjointSet);

  // 0 - 1 - 2 - 3
  disjoint_set.Union(0U, 1U);
  disjoint_set.Union(0U, 2U);
  disjoint_set.Union(1U, 3U);

  // 4 - 8
  disjoint_set.Union(4U, 8U);

  // 5 - 6 - 7
  disjoint_set.Union(5U, 6U);
  disjoint_set.Union(5U, 7U);

  // 0 - 1 - 2 - 3
  // -           -
  REQUIRE(disjoint_set.IsConnected(0U, 3U));

  // 0 - 1 - 2 - 3 | 5 - 6 - 7
  //     -           -
  REQUIRE_FALSE(disjoint_set.IsConnected(1U, 5U));

  // 5 - 6 - 7 | 4 - 8
  //         -       -
  REQUIRE_FALSE(disjoint_set.IsConnected(7U, 8U));
}