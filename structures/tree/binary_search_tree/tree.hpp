#pragma once

#include <structures/tree/binary_search_tree/iterator.hpp>
#include <structures/tree/binary_search_tree/node.hpp>

namespace algo::tree {
template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<Key>>
class BinarySearchTree final {
  using AllocatorTraits = std::allocator_traits<Allocator>;

 public:
  using key_type = Key;
  using value_type = Key;
  using size_type = typename AllocatorTraits::size_type;
  using difference_type = typename AllocatorTraits::difference_type;
  using key_compare = Compare;
  using value_compare = Compare;
  using allocator_type = Allocator;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename AllocatorTraits::pointer;
  using const_pointer = typename AllocatorTraits::const_pointer;

 private:
  // clang-format off
  using node = BinarySearchTreeNode<Key, typename AllocatorTraits::void_pointer>;
  using const_node = const BinarySearchTreeNode<Key, typename AllocatorTraits::void_pointer>;
  using node_allocator = typename AllocatorTraits::template rebind_alloc<node>;
  using node_traits = std::allocator_traits<node_allocator>;
  using node_pointer = typename node_traits::pointer;
  using const_node_pointer = typename node_traits::const_pointer;
  // clang-format on

 public:
  using iterator = BinarySearchTreeIterator<node>;
  using const_iterator = BinarySearchTreeIterator<const_node>;

  explicit BinarySearchTree(Compare comp = Compare(), Allocator alloc = Allocator())
      : compare_(std::move(comp)), allocator_(node_allocator(std::move(alloc))){};

  ~BinarySearchTree() {
    if (root_ != nullptr) {
      DestructorNode(root_);
    }
  }

  iterator begin() noexcept {  // NOLINT(readability-identifier-naming)
    return iterator::MakeInvalid();
  }

  const_iterator begin() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
  }

  const_iterator cbegin() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
  }

  iterator end() noexcept {  // NOLINT(readability-identifier-naming)
    return iterator::MakeInvalid();
  }

  const_iterator end() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
  }

  const_iterator cend() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
  }

  std::pair<iterator, bool> Insert(value_type&& value) {
    return InsertUnique(std::move(value));
  }

  inline bool IsEmpty() const noexcept {
    return Size() == 0;
  }

  inline std::size_t Size() const noexcept {
    return size_;
  }

  node_allocator GetAllocator() const {
    return allocator_;
  }

 private:
  [[gnu::always_inline]] iterator MakeIterator(node_pointer& node) noexcept {
    return iterator(node);
  }

  [[gnu::always_inline]] const_iterator MakeIterator(
      node_pointer const& node) const noexcept {
    return const_iterator(node);
  }

  template <typename T>
  node_pointer ConstructNode(T&& value) requires std::is_same_v<T, value_type> {
    node_pointer new_node = node_traits::allocate(allocator_, 1U);
    node_traits::construct(allocator_, new_node, std::forward<T>(value));
    return new_node;
  }

  void DestructorNode(node_pointer node) {
    node_traits::destroy(allocator_, node);
    node_traits::deallocate(allocator_, node, 1U);
  }

  std::pair<iterator, bool> InsertUnique(value_type&& value) {
    node_pointer new_node = ConstructNode(std::move(value));
    if (root_ == nullptr) {
      root_ = new_node;
      size_++;
      return std::make_pair(MakeIterator(root_), true);
    }
    return {};
  }

 private:
  const key_compare compare_;
  node_allocator allocator_;
  node_pointer root_ = nullptr;
  size_t size_ = 0U;
};
}  // namespace algo::tree