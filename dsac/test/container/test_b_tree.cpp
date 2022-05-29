#include <catch2/catch.hpp>

#include <dsac/container/tree/b_tree.hpp>

TEST_CASE("Проверка B дерева поиска", "[b_tree]") {
  using namespace dsac::tree;

  constexpr int t = 3;
  BTree<int>    tree(t);

  for (const int i : {10, 20, 30, 40, 50, 60, 70, 80, 90}) {
    tree.Insert(i);
  }

  REQUIRE(tree.Contains(90));
  REQUIRE_FALSE(tree.Contains(91));
  REQUIRE(tree.Contains(70));
  REQUIRE_FALSE(tree.Contains(71));
  REQUIRE(tree.Contains(80));
  REQUIRE_FALSE(tree.Contains(81));
  REQUIRE(tree.Contains(40));
  REQUIRE_FALSE(tree.Contains(41));
  REQUIRE(tree.Contains(50));
  REQUIRE_FALSE(tree.Contains(51));
  REQUIRE(tree.Contains(20));
  REQUIRE_FALSE(tree.Contains(21));
  REQUIRE(tree.Contains(10));
  REQUIRE_FALSE(tree.Contains(11));
  REQUIRE(tree.Contains(30));
  REQUIRE_FALSE(tree.Contains(31));
  REQUIRE(tree.Contains(60));
  REQUIRE_FALSE(tree.Contains(61));
}