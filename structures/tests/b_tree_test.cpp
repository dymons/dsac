#include <catch2/catch.hpp>

#include <structures/tree/b_tree.hpp>

TEST_CASE("Проверка B дерева поиска", "[b_tree]") {
  using namespace algo::tree;

  constexpr int t = 3;
  BTree<int> tree(t);

  for (const int i : {10, 20, 30, 40, 50, 60, 70, 80, 90}) {
    tree.Add(i);
  }
}