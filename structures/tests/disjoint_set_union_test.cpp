#include <catch2/catch.hpp>

#include <structures/graph/disjoint_set_union.hpp>

TEST_CASE("Структура данных Disjoint Set Union", "[disjoint]") {
  using namespace algo::graph;

  constexpr std::size_t kSizeDisjointSet = 10U;
  DisjointSet disjoint_set(kSizeDisjointSet);

  disjoint_set.Union(0U, 1U);
  disjoint_set.Union(0U, 2U);
  disjoint_set.Union(1U, 3U);

  disjoint_set.Union(4U, 8U);

  disjoint_set.Union(5U, 6U);
  disjoint_set.Union(5U, 7U);

  REQUIRE(disjoint_set.Find(0U) == disjoint_set.Find(3U));
  REQUIRE_FALSE(disjoint_set.Find(1U) == disjoint_set.Find(5U));
  REQUIRE_FALSE(disjoint_set.Find(7U) == disjoint_set.Find(8U));
}