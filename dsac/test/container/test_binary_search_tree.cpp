#include <catch2/catch.hpp>

#include <dsac/container/tree/bs_tree.hpp>

TEST_CASE("Создание бинарного дерева поиска", "[binary_search_tree][construct]") {
  using namespace dsac;

  SECTION("Проверка метода size для пустого дерева") {
    binary_search_tree<int> tree;
    REQUIRE(tree.size() == 0);
  }
  SECTION("Проверка метода empty для пустого дерева") {
    binary_search_tree<int> tree;
    REQUIRE(tree.IsEmpty());
  }
}

TEST_CASE("Создание итератора для бинарного дерева поиска", "[binary_search_tree][iterator]") {
  using namespace dsac;

  SECTION("Проверка итераторов пустого дерева на равенство") {
    binary_search_tree<int> tree;
    auto                    begin(tree.Begin());
    auto                    end(tree.End());
    REQUIRE(begin == end);
  }
  SECTION("Проверка константных итераторов пустого дерева на равенство") {
    binary_search_tree<int> tree;
    auto                    begin(tree.CBegin());
    auto                    end(tree.CEnd());
    REQUIRE(begin == end);
  }
  SECTION("Получение значения из итератора") {
    binary_search_tree<int, std::less<int>> tree;
    tree.Insert(1);
    auto begin(tree.CBegin());
    auto end(tree.CEnd());
    REQUIRE(begin != end);
    REQUIRE(*begin == 1);
    REQUIRE(++begin == end);
  }
}

TEST_CASE("Добавление элементов в бинарное дерево поиска", "[binary_search_tree][insert]") {
  using namespace dsac;

  SECTION("Добавление элементов в дерево") {
    binary_search_tree<int> tree;
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE(is_added);
    REQUIRE(tree.size() == 1);
  }
  SECTION(
      "Проверка на выполнение условия о хранении уникальных элементов в "
      "дереве"
  ) {
    binary_search_tree<int> tree;
    tree.Insert(0);
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE_FALSE(is_added);
    REQUIRE(tree.size() == 1);
  }
  SECTION("Проверка итерирования по дереву") {
    binary_search_tree<int> tree;
    for (int i{}; i < 100; ++i) {
      tree.Insert(i);
    }
    int value = 0;
    for (auto data : tree) {
      REQUIRE(data == value++);
    }
  }
  SECTION("Проверка итерирования с помощью range-based for") {
    binary_search_tree<int> tree;

    tree.Insert(2);
    tree.Insert(1);
    tree.Insert(3);

    int value = 1;
    for (auto data : tree) {
      REQUIRE(data == value++);
    }
  }

  SECTION("Вставка rvalue значения с помощью метода Insert") {
    struct CopyObject {
      std::shared_ptr<int> counter_copy;
      CopyObject(std::shared_ptr<int> counter_copy)
        : counter_copy(std::move(counter_copy)) {
      }
      CopyObject(CopyObject const& other)
        : counter_copy(other.counter_copy) {
        ++(*counter_copy);
      }
      CopyObject& operator=(CopyObject const& other) {
        counter_copy = other.counter_copy;
        ++(*counter_copy);
        return *this;
      }
      bool operator<(CopyObject const&) const noexcept {
        return true;
      }
    };

    binary_search_tree<CopyObject, std::less<CopyObject>> tree;

    auto counter(std::make_shared<int>());
    tree.Insert({counter});
    REQUIRE(tree.size() == 1);
    REQUIRE(*counter == 1);
  }

  SECTION("Вставка rvalue значения с помощью метода Emplace") {
    struct CopyObject {
      std::shared_ptr<int> counter_copy;
      CopyObject(std::shared_ptr<int> counter_copy)
        : counter_copy(std::move(counter_copy)) {
      }
      CopyObject(CopyObject const& other)
        : counter_copy(other.counter_copy) {
        ++(*counter_copy);
      }
      CopyObject& operator=(CopyObject const& other) {
        counter_copy = other.counter_copy;
        ++(*counter_copy);
        return *this;
      }
      bool operator<(CopyObject const&) const noexcept {
        return true;
      }
    };

    binary_search_tree<CopyObject, std::less<CopyObject>> tree;

    auto counter(std::make_shared<int>());
    tree.Emplace(counter);
    REQUIRE(tree.size() == 1);
    REQUIRE(*counter == 0);
  }
}

TEST_CASE("Удаление всех элементов из бинарного дерева поиска", "[binary_search_tree][clear]") {
  using namespace dsac;

  binary_search_tree<int> tree;
  REQUIRE(tree.IsEmpty());

  for (int i{}; i < 100; ++i) {
    tree.Insert(i);
  }
  REQUIRE_FALSE(tree.IsEmpty());

  tree.Clear();
  REQUIRE(tree.IsEmpty());
}

TEST_CASE("Поиск элементов в бинарном дереве поиска", "[binary_search_tree][find]") {
  using namespace dsac;

  SECTION("Проверка добавленного элемента в дерево") {
    binary_search_tree<int> tree;
    REQUIRE(tree.Find(1) == tree.CEnd());

    tree.Insert(1);
    REQUIRE(tree.Find(1) != tree.CEnd());
  }

  SECTION("Проверка множества добавленных элементов в дерево") {
    binary_search_tree<int> tree;
    for (int i{}; i < 100; ++i) {
      tree.Insert(i);
    }
    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Find(i) != tree.CEnd());
    }
  }

  SECTION("Поиск элемента в дереве после его удаления") {
    binary_search_tree<int> tree;
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Find(1) != tree.CEnd());
    REQUIRE(tree.Erase(1));
    REQUIRE(tree.Find(1) == tree.CEnd());
  }
}

TEST_CASE("Удаление элементов из бинарного дерева поиска", "[binary_search_tree][erase]") {
  using namespace dsac;

  SECTION("Удаление вершины дерева без наличия левой и правой вершины") {
    binary_search_tree<int> tree;

    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.size() == 1);

    REQUIRE(tree.Erase(1));
    REQUIRE(tree.size() == 0);
  }
  SECTION("Удаление самого левого элемента из дерева") {
    binary_search_tree<int> tree;

    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.size() == 2);
    REQUIRE(*tree.Begin() == 1);

    REQUIRE(tree.Erase(1));
    REQUIRE(tree.size() == 1);
    REQUIRE(*tree.Begin() == 2);
  }
  SECTION("Удаление самого правого элемента из дерева") {
    binary_search_tree<int> tree;

    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.size() == 2);

    REQUIRE(tree.Erase(2));
    REQUIRE(tree.size() == 1);
  }
  SECTION("Удаление вершины дерева с наличием левой и правой вершины") {
    binary_search_tree<int> tree;

    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Insert(3).second);
    REQUIRE(tree.size() == 3);

    REQUIRE(tree.Erase(2));
    REQUIRE(tree.size() == 2);
  }
  SECTION("Удаление всех элементов") {
    binary_search_tree<int> tree;

    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Insert(i).second);
    }
    REQUIRE(tree.size() == 100);

    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Erase(i));
    }
    REQUIRE(tree.size() == 0);
  }
}