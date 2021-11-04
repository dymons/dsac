#include <catch2/catch.hpp>

#include <structures/tree/rb_tree.hpp>

TEST_CASE("Проверка выполнения корректности поворотов для красно-черное дерево",
          "[rb_tree_rotate]") {
  using namespace algo::tree;

  SECTION("Проверка корректности выполнения малого правого вращения") {
    RBTree<int> tree;
    for (const int i : {3, 2, 1}) {
      tree.Insert(i);
    }

    int index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Проверка корректности выполнения малого левого вращения") {
    RBTree<int> tree;
    for (const int i : {1, 2, 3}) {
      tree.Insert(i);
    }

    int index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }
}

TEST_CASE("Корректность построения красно-черного дерева", "[rb_tree_build]") {
  using namespace algo::tree;

  SECTION("Проверка высоты красно-черного дерева") {
    RBTree<int> tree;
    for (const int i : {1}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 0);
  }

  SECTION("Построение AVL дерева на отсортированном массиве") {
    RBTree<int> tree;
    tree.Insert(1);
    REQUIRE(tree.Depth() == 0);
    tree.Insert(2);
    REQUIRE(tree.Depth() == 1);
    tree.Insert(3);
    REQUIRE(tree.Depth() == 1);
    tree.Insert(4);
    REQUIRE(tree.Depth() == 2);
    tree.Insert(5);
    REQUIRE(tree.Depth() == 2);
    tree.Insert(6);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(7);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(8);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(9);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(10);
    REQUIRE(tree.Depth() == 4);

    int index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }
}