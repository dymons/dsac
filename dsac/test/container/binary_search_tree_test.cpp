#include "catch2/catch.hpp"

#include <dsac/container/tree/bs_tree.hpp>

TEST_CASE("Создание бинарного дерева поиска", "[binary_search_tree][construct]") {
  using namespace dsac::tree;

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
  using namespace dsac::tree;

  SECTION("Проверка итераторов пустого дерева на равенство") {
    BinarySearchTree<int> tree;
    auto begin(tree.Begin());
    auto end(tree.End());
    REQUIRE(begin == end);
  }
  SECTION("Проверка константных итераторов пустого дерева на равенство") {
    BinarySearchTree<int> tree;
    auto begin(tree.CBegin());
    auto end(tree.CEnd());
    REQUIRE(begin == end);
  }
  SECTION("Получение значения из итератора") {
    BinarySearchTree<int, std::less<int>> tree;
    tree.Insert(1);
    auto begin(tree.CBegin());
    auto end(tree.CEnd());
    REQUIRE(begin != end);
    REQUIRE(*begin == 1);
    REQUIRE(++begin == end);
  }
}

TEST_CASE("Добавление элеметнов в бинарное дерево поиска",
          "[binary_search_tree][insert]") {
  using namespace dsac::tree;

  SECTION("Добавление элементов в дерево") {
    BinarySearchTree<int> tree;
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE(is_added);
    REQUIRE(tree.Size() == 1);
  }
  SECTION("Проверка на выполнение условия о хранении уникальных элементов в дереве") {
    BinarySearchTree<int> tree;
    tree.Insert(0);
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE_FALSE(is_added);
    REQUIRE(tree.Size() == 1);
  }
  SECTION("Проверка итерирования по дереву") {
    BinarySearchTree<int> tree;
    for (int i{}; i < 100; ++i) {
      tree.Insert(i);
    }
    int value = 0;
    for (auto data : tree) {
      REQUIRE(data == value++);
    }
  }
  SECTION("Проверка итерирования с помощью range-based for") {
    BinarySearchTree<int> tree;

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
      CopyObject(CopyObject const& other) : counter_copy(other.counter_copy) {
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

    BinarySearchTree<CopyObject, std::less<CopyObject>> tree;

    auto counter(std::make_shared<int>());
    tree.Insert({counter});
    REQUIRE(tree.Size() == 1);
    REQUIRE(*counter == 1);
  }

  SECTION("Вставка rvalue значения с помощью метода Empalce") {
    struct CopyObject {
      std::shared_ptr<int> counter_copy;
      CopyObject(std::shared_ptr<int> counter_copy)
          : counter_copy(std::move(counter_copy)) {
      }
      CopyObject(CopyObject const& other) : counter_copy(other.counter_copy) {
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

    BinarySearchTree<CopyObject, std::less<CopyObject>> tree;

    auto counter(std::make_shared<int>());
    tree.Emplace(counter);
    REQUIRE(tree.Size() == 1);
    REQUIRE(*counter == 0);
  }
}

TEST_CASE("Удаление всух элеметнов из бинарного дерева поиска",
          "[binary_search_tree][clear]") {
  using namespace dsac::tree;

  BinarySearchTree<int> tree;
  REQUIRE(tree.IsEmpty());

  for (int i{}; i < 100; ++i) {
    tree.Insert(i);
  }
  REQUIRE_FALSE(tree.IsEmpty());

  tree.Clear();
  REQUIRE(tree.IsEmpty());
}

TEST_CASE("Поиск элементов в бинарном дереве поиска", "[binary_search_tree][find]") {
  using namespace dsac::tree;

  SECTION("Проверка добавленного элемента в дерево") {
    BinarySearchTree<int> tree;
    REQUIRE(tree.Find(1) == tree.CEnd());

    tree.Insert(1);
    REQUIRE(tree.Find(1) != tree.CEnd());
  }

  SECTION("Проверка множества добавленных элементов в дерево") {
    BinarySearchTree<int> tree;
    for (int i{}; i < 100; ++i) {
      tree.Insert(i);
    }
    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Find(i) != tree.CEnd());
    }
  }

  SECTION("Поиск элемента в дереве после его удаления") {
    BinarySearchTree<int> tree;
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Find(1) != tree.CEnd());
    REQUIRE(tree.Erase(1));
    REQUIRE(tree.Find(1) == tree.CEnd());
  }
}

TEST_CASE("Удаление элементов из бинарного дерева поиска",
          "[binary_search_tree][erase]") {
  using namespace dsac::tree;

  SECTION("Удаление вершины дерева без наличия левой и правой вершины") {
    BinarySearchTree<int> tree;

    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Size() == 1);

    REQUIRE(tree.Erase(1));
    REQUIRE(tree.Size() == 0);
  }
  SECTION("Удаление самого левого элемента из дерева") {
    BinarySearchTree<int> tree;

    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Size() == 2);
    REQUIRE(*tree.Begin() == 1);

    REQUIRE(tree.Erase(1));
    REQUIRE(tree.Size() == 1);
    REQUIRE(*tree.Begin() == 2);
  }
  SECTION("Удаление самого правого элемента из дерева") {
    BinarySearchTree<int> tree;

    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.Size() == 2);

    REQUIRE(tree.Erase(2));
    REQUIRE(tree.Size() == 1);
  }
  SECTION("Удаление вершины дерева с наличием левой и правой вершины") {
    BinarySearchTree<int> tree;

    REQUIRE(tree.Insert(2).second);
    REQUIRE(tree.Insert(1).second);
    REQUIRE(tree.Insert(3).second);
    REQUIRE(tree.Size() == 3);

    REQUIRE(tree.Erase(2));
    REQUIRE(tree.Size() == 2);
  }
  SECTION("Удаление всех элементов") {
    BinarySearchTree<int> tree;

    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Insert(i).second);
    }
    REQUIRE(tree.Size() == 100);

    for (int i{}; i < 100; ++i) {
      REQUIRE(tree.Erase(i));
    }
    REQUIRE(tree.Size() == 0);
  }
}

namespace {
template <typename T>
class AllocatorWithCounters : public std::allocator<T> {
 public:
  template <typename U>
  struct rebind {
    using other = AllocatorWithCounters<U>;
  };

  using size_type = std::size_t;
  using pointer = T*;
  using const_pointer = const pointer*;

  size_type alloc_entities = 0U;
  size_type dealloc_entities = 0U;

  pointer allocate(size_type n) {  // NOLINT
    alloc_entities += n;
    return std::allocator<T>::allocate(n);
  }

  void deallocate(pointer p, size_type n) {  // NOLINT
    dealloc_entities += n;
    return std::allocator<T>::deallocate(p, n);
  }
};
}  // namespace

TEST_CASE("Управление ресурсами в бинарном дереве поиска",
          "[binary_search_tree][allocator]") {
  using namespace dsac::tree;

  using Tree = BinarySearchTree<int, std::less<int>, AllocatorWithCounters<int>>;
  using TreePtr = std::shared_ptr<Tree>;

  SECTION("Отсутствие выделеняи памяти при создании пустого бинарного дерева") {
    TreePtr shared_tree(new Tree{}, [](Tree* tree) {
      if (tree) {
        tree->~Tree();
        auto allocator = tree->GetAllocator();
        REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
        std::free(tree);
      }
    });
  }
  SECTION("Выделение и освобождение памяти для одного добавленного элемента") {
    TreePtr shared_tree(new Tree{}, [](Tree* tree) {
      if (tree) {
        tree->~Tree();
        auto allocator = tree->GetAllocator();
        REQUIRE(allocator.dealloc_entities != 0);
        REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
        std::free(tree);
      }
    });

    shared_tree->Insert(0);

    auto allocator = shared_tree->GetAllocator();
    REQUIRE(allocator.alloc_entities != 0);
  }
  SECTION("Выделение и освобождение памяти для множества добавленных элементов") {
    TreePtr shared_tree(new Tree{}, [](Tree* tree) {
      if (tree) {
        tree->~Tree();
        auto allocator = tree->GetAllocator();
        REQUIRE(allocator.dealloc_entities != 0);
        REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
        std::free(tree);
      }
    });

    constexpr int kNumOfElements = 100;
    for (int i{}; i < kNumOfElements; ++i) {
      shared_tree->Insert(i);
    }

    auto allocator = shared_tree->GetAllocator();
    REQUIRE(allocator.alloc_entities == kNumOfElements);
  }
  SECTION("Выделение и освобождение памяти с помощью метода Clear()") {
    TreePtr shared_tree(new Tree{}, [](Tree* tree) {
      if (tree) {
        tree->~Tree();
        auto allocator = tree->GetAllocator();
        REQUIRE(allocator.dealloc_entities != 0);
        REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
        std::free(tree);
      }
    });

    constexpr int kNumOfElements = 100;
    for (int i{}; i < kNumOfElements; ++i) {
      shared_tree->Insert(i);
    }

    auto allocator = shared_tree->GetAllocator();
    REQUIRE(allocator.alloc_entities >= kNumOfElements);

    shared_tree->Clear();
  }

  SECTION("Выделение и освобождение памяти с помощью метода Erase()") {
    TreePtr shared_tree(new Tree{}, [](Tree* tree) {
      if (tree) {
        tree->~Tree();
        auto allocator = tree->GetAllocator();
        REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
        std::free(tree);
      }
    });

    constexpr int kNumOfElements = 100;
    for (int i{}; i < kNumOfElements; ++i) {
      shared_tree->Insert(i);
    }
    REQUIRE(shared_tree->GetAllocator().alloc_entities == kNumOfElements);

    for (int i{}; i < kNumOfElements; ++i) {
      shared_tree->Erase(i);
    }
    REQUIRE(shared_tree->GetAllocator().dealloc_entities == kNumOfElements);
  }
}