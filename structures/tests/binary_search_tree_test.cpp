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

  TreePtr shared_tree(new Tree{}, [](Tree* tree) {
    if (tree) {
      tree->~Tree();
      auto allocator = tree->GetAllocator();
      REQUIRE(allocator.dealloc_entities == 0);
      REQUIRE(allocator.alloc_entities == allocator.dealloc_entities);
      std::free(tree);
    }
  });

  auto allocator = shared_tree->GetAllocator();
  REQUIRE(allocator.alloc_entities == 0);

  shared_tree.reset();
}