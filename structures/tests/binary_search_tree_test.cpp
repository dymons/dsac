#include <catch2/catch.hpp>

#include <structures/tree/binary_search_tree/tree.hpp>

TEST_CASE("Создание бинарного дерева поиска", "[binary_search_tree][construct]") {
  using namespace algo::tree;

  SECTION("Проверка метода size для пустого дерева") {
    BinarySearchTree<int> tree;
    REQUIRE(tree.Size() == 0);
  }
  SECTION("Проверка метода empty для пустого дерева") {
    BinarySearchTree<int> tree;
    REQUIRE(tree.IsEmpty());
  }
}

TEST_CASE("Создание итератора для бинарного дерева поиска",
          "[binary_search_tree][iterator]") {
  using namespace algo::tree;

  SECTION("Проверка итераторов пустого дерева на равенство") {
    BinarySearchTree<int> tree;
    auto begin(tree.begin());
    auto end(tree.end());
    REQUIRE(begin == end);
  }
  SECTION("Проверка константных итераторов пустого дерева на равенство") {
    BinarySearchTree<int> tree;
    auto begin(tree.cbegin());
    auto end(tree.cend());
    REQUIRE(begin == end);
  }
}