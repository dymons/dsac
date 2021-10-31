#include <catch2/catch.hpp>

#include <random>
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

  SECTION("Проверка высоты AVL дерева") {
    AVLTree<int> tree;
    for (const int i : {1}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 0);
  }

  SECTION("Построение AVL дерева на отсортированном массиве") {
    AVLTree<int> tree;
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
    REQUIRE(tree.Depth() == 2);
    tree.Insert(7);
    REQUIRE(tree.Depth() == 2);
    tree.Insert(8);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(9);
    REQUIRE(tree.Depth() == 3);
    tree.Insert(10);
    REQUIRE(tree.Depth() == 3);

    int index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение AVL дерева на обратно отсортированном массиве") {
    AVLTree<int> tree;
    for (const int i : {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 3);

    int index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение AVL дерева с отрицательными значениями") {
    AVLTree<int> tree;
    for (const int i : {-5, -4, -3, -2, -1, 1, 2, 3, 4, 5}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 3);

    int index = 0;
    const std::vector<int> expected{-5, -4, -3, -2, -1, 1, 2, 3, 4, 5};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение AVL дерева с дубликатами") {
    AVLTree<int> tree;
    for (const int i : {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 3);

    int index = 0;
    const std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Построение пустого AVL дерева") {
    AVLTree<int> tree;
    REQUIRE(tree.Depth() == -1);

    int index = 0;
    tree.Visit([&index](int data) {
      index++;
    });
    REQUIRE(index == 0);
  }
}

TEST_CASE("Корректность удаления элементов из AVL дерева", "[avl_tree_delete]") {
  using namespace algo::tree;

  SECTION("Удаление элементов из пустого дерева") {
    AVLTree<int> tree;
    REQUIRE_NOTHROW(tree.Delete(1));
  }

  SECTION("Удаление вершины дерева") {
    AVLTree<int> tree;
    for (const int i : {1, 2, 4, 3}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 2);

    tree.Delete(2);
    REQUIRE_FALSE(tree.Contains(2));

    int index = 0;
    const std::vector<int> expected{1, 3, 4};
    tree.Visit([&index, &expected](int data) {
      REQUIRE(index < expected.size());
      REQUIRE(data == expected[index++]);
    });
  }

  SECTION("Удаление всех элементов из AVL дерева") {
    AVLTree<int> tree;
    for (const int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
      tree.Insert(i);
    }
    REQUIRE(tree.Depth() == 3);

    for (const int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
      tree.Delete(i);
    }
    REQUIRE(tree.Depth() == -1);

    int index = 0;
    tree.Visit([&index](int data) {
      ++index;
    });
    REQUIRE(index == 0);
  }

  SECTION("Добавление/Удаление случайных элементов в AVL дерева") {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(-1000, 1000);

    constexpr int count_samples = 2000;
    std::vector<int> buffer;
    buffer.reserve(count_samples);

    AVLTree<int> tree;
    for (int i = 0; i < count_samples; ++i) {
      buffer.push_back(distribution(generator));
      tree.Insert(buffer.back());
    }

    for (int data : buffer) {
      tree.Delete(data);
    }
    REQUIRE(tree.Depth() == -1);

    int index = 0;
    tree.Visit([&index](int data) {
      ++index;
    });
    REQUIRE(index == 0);
  }
}