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
      : compare_(std::move(comp)),
        allocator_(node_allocator(std::move(alloc))),
        size_(0),
        header_(ConstructNode(value_type{})) {
    header_->left_ = header_;
    header_->right_ = header_;
  };

  ~BinarySearchTree() {
    node_pointer next_to_destroy = nullptr;
    for (node_pointer root = GetRootNode(); root != nullptr; root = next_to_destroy) {
      if (root->left_ == nullptr) {
        next_to_destroy = root->right_;
        DestroyNode(root);
      } else {
        next_to_destroy = root->left_;
        root->left_ = next_to_destroy->right_;
        next_to_destroy->right_ = root;
      }
    }

    DestroyNode(header_);
  }

  iterator begin() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(GetLeftmostNode());
  }

  const_iterator begin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(GetLeftmostNode());
  }

  const_iterator cbegin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(GetLeftmostNode());
  }

  iterator end() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(header_);
  }

  const_iterator end() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(header_);
  }

  const_iterator cend() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIterator(header_);
  }

  std::pair<iterator, bool> Insert(value_type const& value) {
    return InsertUnique(value);
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
  [[gnu::always_inline]] iterator MakeIterator(node_pointer node) noexcept {
    return iterator(node);
  }

  [[gnu::always_inline]] const_iterator MakeIterator(
      const_node_pointer node) const noexcept {
    return const_iterator(node);
  }

  template <typename T>
  node_pointer ConstructNode(T value) requires std::is_same_v<T, value_type> {
    node_pointer new_node = node_traits::allocate(allocator_, 1U);
    node_traits::construct(allocator_, new_node, std::move(value));
    return new_node;
  }

  [[gnu::always_inline]] void DestroyNode(node_pointer node) {
    node_traits::destroy(allocator_, node);
    node_traits::deallocate(allocator_, node, 1U);
  }

  std::pair<iterator, bool> InsertUnique(value_type const& value) {
    node_pointer inserted_node = ConstructNode(value);
    node_pointer root = GetRootNode();
    if (root == nullptr) {
      header_->parent_ = inserted_node;
      header_->left_ = inserted_node;
      header_->right_ = inserted_node;
      inserted_node->parent_ = header_;
      size_++;
      return std::make_pair(MakeIterator(header_), true);
    }

    node_pointer parent = root;
    node_pointer parent_before = root;
    while (parent != nullptr) {
      parent_before = parent;
      if (compare_(inserted_node->key_, parent->key_)) {
        parent = parent->left_;
      } else if (compare_(parent->key_, inserted_node->key_)) {
        parent = parent->right_;
      } else {
        return std::make_pair(MakeIterator(parent), false);
      }
    }

    if (compare_(inserted_node->key_, parent_before->key_)) {
      parent_before->left_ = inserted_node;
    } else {
      parent_before->right_ = inserted_node;
    }

    inserted_node->parent_ = parent_before;
    size_++;

    if (compare_(inserted_node->key_, GetLeftmostNode()->key_)) {
      header_->left_ = inserted_node;
    }
    if (!(compare_(inserted_node->key_, GetRightmostNode()->key_)) &&
        !(compare_(GetRightmostNode()->key_, inserted_node->key_))) {
      header_->right_ = inserted_node;
      inserted_node->right_ = header_;
    }

    return std::make_pair(MakeIterator(inserted_node), true);
  }

  [[gnu::always_inline]] node_pointer& GetRootNode() {
    return header_->parent_;
  }

  [[gnu::always_inline]] const_node_pointer& GetRootNode() const {
    return header_->parent_;
  }

  [[gnu::always_inline]] node_pointer& GetLeftmostNode() noexcept {
    return header_->left_;
  }

  [[gnu::always_inline]] const_node_pointer GetLeftmostNode() const noexcept {
    return header_->left_;
  }

  [[gnu::always_inline]] node_pointer& GetRightmostNode() noexcept {
    return header_->right_;
  }

  [[gnu::always_inline]] const_node_pointer GetRightmostNode() const noexcept {
    return header_->right_;
  }

 private:
  const key_compare compare_;
  node_allocator allocator_;
  node_pointer header_;
  size_t size_;
};
}  // namespace algo::tree