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
  SECTION("Получение значения из итератора") {
    BinarySearchTree<int, std::less<int>> tree;
    tree.Insert(1);
    auto const begin(tree.cbegin());
    auto const end(tree.cend());
    REQUIRE(begin != end);
    REQUIRE(*begin == 1);
  }
}

TEST_CASE("Добавление элеметнов в бинарное дерево поиска",
          "[binary_search_tree][insert]") {
  using namespace algo::tree;

  SECTION("Добавление элементов в дерево") {
    BinarySearchTree<int> tree;
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE(is_added);
    REQUIRE(tree.Size() == 1);
  }
  SECTION("Проверка на выполнение условия о хранении уникальных элементов в дереве") {
    BinarySearchTree<int> tree;
    [[maybe_unused]] auto const pair = tree.Insert(0);
    auto const [_, is_added] = tree.Insert(0);
    REQUIRE_FALSE(is_added);
    REQUIRE(tree.Size() == 1);
  }
}

TEST_CASE("Удаление всух элеметнов из бинарного дерева поиска",
          "[binary_search_tree][clear]") {
  using namespace algo::tree;

  BinarySearchTree<int> tree;
  REQUIRE(tree.IsEmpty());

  for (int i{}; i < 100; ++i) {
    tree.Insert(i);
  }
  REQUIRE_FALSE(tree.IsEmpty());

  tree.Clear();
  REQUIRE(tree.IsEmpty());
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
  using namespace algo::tree;

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

    shared_tree.reset();
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

    shared_tree.reset();
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
    REQUIRE(allocator.alloc_entities >= kNumOfElements);

    shared_tree.reset();
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
    shared_tree.reset();
  }
}