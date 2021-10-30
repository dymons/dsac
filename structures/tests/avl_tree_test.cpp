#include <catch2/catch.hpp>

#include <vector>
#include <structures/tree/avl_tree.hpp>

TEST_CASE("Проверка выполнения корректности поворотов для AVL дерева",
          "[avl_tree_rotate]") {
  using namespace algo::tree;

  SECTION("Проверка корректности выполнения малого правого вращения") {
    AVLTree<int> tree;
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
    AVLTree<int> tree;
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

  SECTION("Проверка корректности выполнения большого левого вращения") {
    AVLTree<int> tree;
    for (const int i : {1, 3, 2}) {
      tree.Insert(i);
    }

    int index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Проверка корректности выполнения большого правого вращения") {
    AVLTree<int> tree;
    for (const int i : {3, 1, 2}) {
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

TEST_CASE("Корректность построения AVL дерева", "[avl_tree_build]") {
  using namespace algo::tree;

  AVLTree<int> tree;
  for (const int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
    tree.Insert(i);
  }

  int index = 0;
  const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  tree.Visit([&index, &expected](int data) {
    REQUIRE(index < expected.size());
    REQUIRE(data == expected[index++]);
  });
}