#include <catch2/catch.hpp>

#include <structures/tree/avl_tree.hpp>

TEST_CASE("Сбалансированное бинарное дерево поиска", "[avl_tree]") {
  using namespace algo::tree;
  AVLTree<int> tree;

  for (const int i : {3, 2, 1}) {
    tree.Insert(i);
  }
}