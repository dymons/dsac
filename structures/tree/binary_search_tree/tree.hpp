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
        root_(nullptr){};

  ~BinarySearchTree() {
    Clear();
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
    return iterator::MakeInvalid();
  }

  const_iterator end() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
  }

  const_iterator cend() const noexcept {  // NOLINT(readability-identifier-naming)
    return const_iterator::MakeInvalid();
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

  void Clear() {
    DestroySubtree(root_);
  }

  const_iterator Find(key_type const& key) const {
    return FindUnique(key);
  }

  bool Erase(key_type const& key) {
    return EraseUnique(key);
  }

 private:
  [[gnu::always_inline]] iterator MakeIterator(node_pointer node) noexcept {
    return iterator(node);
  }

  [[gnu::always_inline]] const_iterator MakeIterator(
      const_node_pointer node) const noexcept {
    return const_iterator(const_cast<node_pointer>(node));
  }

  template <typename T>
  node_pointer ConstructNode(T value) requires std::is_same_v<T, value_type> {
    node_pointer new_node = node_traits::allocate(allocator_, 1U);
    node_traits::construct(allocator_, new_node, std::move(value));
    return new_node;
  }

  [[gnu::always_inline]] void DestroyNode(node_pointer& node) {
    node_traits::destroy(allocator_, node);
    node_traits::deallocate(allocator_, node, 1U);
    node = nullptr;
  }

  std::pair<iterator, bool> InsertUnique(value_type const& value) {
    node_pointer inserted_node = ConstructNode(value);
    if (root_ == nullptr) {
      root_ = inserted_node;
      size_++;
      return std::make_pair(MakeIterator(root_), true);
    }

    node_pointer parent = root_;
    node_pointer parent_before = root_;
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

    return std::make_pair(MakeIterator(inserted_node), true);
  }

  [[gnu::always_inline]] node_pointer GetLeftmostNode() noexcept {
    node_pointer leftmost = root_;
    while (leftmost && leftmost->left_) {
      leftmost = leftmost->left_;
    }
    return leftmost;
  }

  [[gnu::always_inline]] const_node_pointer GetLeftmostNode() const noexcept {
    const_node_pointer leftmost = root_;
    while (leftmost && leftmost->left_) {
      leftmost = leftmost->left_;
    }
    return leftmost;
  }

  [[gnu::always_inline]] node_pointer GetRightmostNode() noexcept {
    node_pointer rightmost = root_;
    while (rightmost && rightmost->right_) {
      rightmost = rightmost->right_;
    }
    return rightmost;
  }

  [[gnu::always_inline]] const_node_pointer GetRightmostNode() const noexcept {
    const_node_pointer rightmost = root_;
    while (rightmost && rightmost->right_) {
      rightmost = rightmost->right_;
    }
    return rightmost;
  }

  void DestroySubtree(node_pointer& root) {
    node_pointer next_to_destroy = nullptr;
    for (; root != nullptr; root = next_to_destroy) {
      if (root->left_ == nullptr) {
        next_to_destroy = root->right_;
        DestroyNode(root);
      } else {
        next_to_destroy = root->left_;
        root->left_ = next_to_destroy->right_;
        next_to_destroy->right_ = root;
      }
    }

    size_ = 0;
  }

  iterator FindUnique(key_type const& key) {
    node_pointer root = root_;
    while (root != nullptr) {
      if (compare_(key, root->key_)) {
        root = root->left_;
      } else if (compare_(root->key_, key)) {
        root = root->right_;
      } else {
        return MakeIterator(root);
      }
    }

    return end();
  }

  const_iterator FindUnique(key_type const& key) const {
    const_node_pointer root = root_;
    while (root != nullptr) {
      if (compare_(key, root->key_)) {
        root = root->left_;
      } else if (compare_(root->key_, key)) {
        root = root->right_;
      } else {
        return MakeIterator(root);
      }
    }

    return cend();
  }

  bool EraseUnique(key_type const& key) {
    if (iterator it = FindUnique(key); it != end()) {
      bool const has_left_node = it.current_node_->left_ != nullptr;
      bool const has_right_node = it.current_node_->right_ != nullptr;
      if (!has_left_node && !has_right_node) {
        if (it.current_node_ == root_) {
          DestroyNode(root_);
        } else {
          if (it.current_node_->parent_->left_ == it.current_node_) {
            it.current_node_->parent_->left_ = nullptr;
          } else {
            it.current_node_->parent_->right_ = nullptr;
          }

          DestroyNode(it.current_node_);
        }
      } else if (has_left_node && has_right_node) {
        node_pointer current_leftmost = it.current_node_->right_;
        while (current_leftmost->left_ != nullptr) {
          current_leftmost = current_leftmost->left_;
        }

        it.current_node_->key_ = std::move(current_leftmost->key_);
        if (current_leftmost->parent_->left_ == current_leftmost) {
          current_leftmost->parent_->left_ = current_leftmost->right_;
        } else {
          current_leftmost->parent_->right_ = current_leftmost->right_;
        }

        DestroyNode(current_leftmost);
      } else {
        node_pointer child_node = has_left_node ? it.current_node_->left_ : it.current_node_->right_;
        if (it.current_node_ == root_) {
          root_ = child_node;
        } else {
          if (it.current_node_ == it.current_node_->parent_->left_) {
            it.current_node_->parent_->left_ = child_node;
          } else {
            it.current_node_->parent_->right_ = child_node;
          }
        }

        DestroyNode(it.current_node_);
      }

      --size_;
      return true;
    }

    return false;
  }

 private:
  const key_compare compare_;
  node_allocator allocator_;
  node_pointer root_;
  size_t size_;
};
}  // namespace algo::tree