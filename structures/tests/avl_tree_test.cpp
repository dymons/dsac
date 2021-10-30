#include <catch2/catch.hpp>

#include <vector>
#include <structures/tree/avl_tree.hpp>

TEST_CASE("Сбалансированное бинарное дерево поиска", "[avl_tree]") {
  using namespace algo::tree;

  SECTION("Малое правое вращение") {
    AVLTree<int> tree;
    for (const int i : {3, 2, 1}) {
      tree.Insert(i);
    }

    int index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Малое левое вращение") {
    AVLTree<int> tree;
    for (const int i : {1, 2, 3}) {
      tree.Insert(i);
    }

    int index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(data == expected[index++]);
    });
  }
}