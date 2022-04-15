#include "catch2/catch.hpp"

#include <dsac/container/tree/rb_tree.hpp>
#include <random>
#include <vector>

namespace {
template <typename T>
[[nodiscard]] bool IsInvariantRight(const dsac::tree::RBTree<T>& tree) {
  return tree.MaxDepth() / 2 <= tree.MinDepth();
}
}  // namespace

TEST_CASE(
    "Проверка выполнения корректности поворотов для красно-черное дерево",
    "[rb_tree_rotate]") {
  using namespace dsac::tree;

  SECTION("Проверка корректности выполнения малого правого вращения") {
    RBTree<int> tree;
    for (const int i : {3, 2, 1}) {
      tree.Insert(i);
    }

    int                    index = 0;
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

    int                    index = 0;
    const std::vector<int> expected{1, 2, 3};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }
}

TEST_CASE("Корректность построения красно-черного дерева", "[rb_tree_build]") {
  using namespace dsac::tree;

  SECTION("Проверка высоты красно-черного дерева") {
    RBTree<int> tree;
    for (const int i : {1}) {
      tree.Insert(i);
    }
    REQUIRE(tree.MaxDepth() == 0);
    REQUIRE(tree.MinDepth() == 0);
  }

  SECTION("Построение AVL дерева на отсортированном массиве") {
    RBTree<int> tree;
    tree.Insert(1);
    REQUIRE(tree.MaxDepth() == 0);
    REQUIRE(tree.MinDepth() == 0);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(1));
    tree.Insert(2);
    REQUIRE(tree.MaxDepth() == 1);
    REQUIRE(tree.MinDepth() == 0);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(2));
    tree.Insert(3);
    REQUIRE(tree.MaxDepth() == 1);
    REQUIRE(tree.MinDepth() == 1);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(3));
    tree.Insert(4);
    REQUIRE(tree.MaxDepth() == 2);
    REQUIRE(tree.MinDepth() == 1);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(4));
    tree.Insert(5);
    REQUIRE(tree.MaxDepth() == 2);
    REQUIRE(tree.MinDepth() == 1);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(5));
    tree.Insert(6);
    REQUIRE(tree.MaxDepth() == 3);
    REQUIRE(tree.MinDepth() == 1);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(6));
    tree.Insert(7);
    REQUIRE(tree.MaxDepth() == 3);
    REQUIRE(tree.MinDepth() == 1);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(7));
    tree.Insert(8);
    REQUIRE(tree.MaxDepth() == 3);
    REQUIRE(tree.MinDepth() == 2);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(8));
    tree.Insert(9);
    REQUIRE(tree.MaxDepth() == 3);
    REQUIRE(tree.MinDepth() == 2);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(9));
    tree.Insert(10);
    REQUIRE(tree.MaxDepth() == 4);
    REQUIRE(tree.MinDepth() == 2);
    REQUIRE(IsInvariantRight(tree));
    REQUIRE(tree.Contains(10));

    int                    index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение AVL дерева на обратно отсортированном массиве") {
    RBTree<int> tree;
    for (const int i : {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}) {
      tree.Insert(i);
    }
    REQUIRE(tree.MaxDepth() == 4);

    int                    index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение AVL дерева с отрицательными значениями") {
    RBTree<int> tree;
    for (const int i : {-5, -4, -3, -2, -1, 1, 2, 3, 4, 5}) {
      tree.Insert(i);
    }
    REQUIRE(tree.MaxDepth() == 4);

    int                    index = 0;
    const std::vector<int> expected{-5, -4, -3, -2, -1, 1, 2, 3, 4, 5};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение пустого AVL дерева") {
    RBTree<int> tree;
    REQUIRE(tree.MaxDepth() == -1);

    int index = 0;
    tree.Visit([&index](int data) { index++; });
    REQUIRE(index == 0);
  }

  SECTION("Добавление/Удаление случайных элементов в AVL дерева") {
    std::random_device              random_device;
    std::mt19937                    generator(random_device());
    std::uniform_int_distribution<> distribution(-10000, 10000);

    constexpr int count_samples = 20000;
    RBTree<int>   tree;
    for (int i = 0; i < count_samples; ++i) {
      tree.Insert(distribution(generator));
      REQUIRE(IsInvariantRight(tree));
    }
  }
}