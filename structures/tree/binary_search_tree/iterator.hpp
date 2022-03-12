#pragma once

#include <iterator>

namespace algo::tree {
template <typename BinarySearchTreeNode>
class BinarySearchTreeIterator final
    : std::iterator<std::bidirectional_iterator_tag,
                    typename BinarySearchTreeNode::pointer> {
  using key_type = typename BinarySearchTreeNode::key_type;
  using node_pointer = typename BinarySearchTreeNode::pointer;
  using const_node_pointer = typename BinarySearchTreeNode::const_pointer;

  template <typename, typename, typename>
  friend class BinarySearchTree;

 public:
  using iterator_category = const std::bidirectional_iterator_tag;
  using value_type = typename BinarySearchTreeNode::value_type;
  using const_reference = typename BinarySearchTreeNode::const_reference;

 private:
  bool IsLeftChild(node_pointer node) const noexcept {
    return node == node->parent_->left_;
  }

  node_pointer FindMinNode(node_pointer node) const noexcept {
    while (node != nullptr && node->left_ != nullptr) {
      node = node->left_;
    }
    return node;
  }

  node_pointer FindMaxNode(node_pointer node) const noexcept {
    while (node != nullptr && node->right_ != nullptr) {
      node = node->right_;
    }
    return node;
  }

  node_pointer FindNextNode(node_pointer node) const {
    if (node != nullptr && node->right_ != nullptr) {
      return FindMinNode(node->right_);
    }

    while (node->parent_ && !IsLeftChild(node)) {
      node = node->parent_;
    }

    return node->parent_;
  }

  node_pointer current_node_ = nullptr;

 public:
  static BinarySearchTreeIterator MakeInvalid() {
    return {};
  }

  const_reference operator*() const {
    return current_node_->key_;
  }

  BinarySearchTreeIterator& operator++() {
    current_node_ = FindNextNode(current_node_);
    return *this;
  }

  BinarySearchTreeIterator operator++(int) {
    auto latest_iterator = *this;
    ++*this;
    return latest_iterator;
  }

  BinarySearchTreeIterator operator--() = delete;
  BinarySearchTreeIterator operator--(int) = delete;

  friend bool operator==(const BinarySearchTreeIterator& lhs,
                         const BinarySearchTreeIterator& rhs) {
    return lhs.current_node_ == rhs.current_node_;
  }

  friend bool operator!=(const BinarySearchTreeIterator& lhs,
                         const BinarySearchTreeIterator& rhs) {
    return !(lhs == rhs);
  }

  BinarySearchTreeIterator() = default;
  BinarySearchTreeIterator(const BinarySearchTreeIterator&) = default;
  BinarySearchTreeIterator(BinarySearchTreeIterator&&) = default;
  BinarySearchTreeIterator& operator=(const BinarySearchTreeIterator&) = default;
  BinarySearchTreeIterator& operator=(BinarySearchTreeIterator&&) = default;
  ~BinarySearchTreeIterator() = default;

 private:
  explicit BinarySearchTreeIterator(node_pointer node) : current_node_(node) {
  }
};
}  // namespace algo::tree