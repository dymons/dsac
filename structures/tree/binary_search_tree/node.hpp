#pragma once

#include <memory>

namespace algo::tree {
template <typename Key, typename VoidPointer>
class BinarySearchTreeNode final {
 public:
  // clang-format off
    using key_type = Key;
    using value_type = Key;
    using const_reference = const value_type&;
    using pointer_traits = typename std::pointer_traits<VoidPointer>::template rebind<BinarySearchTreeNode>;
    using pointer = typename std::pointer_traits<VoidPointer>::template rebind<BinarySearchTreeNode>;
    using const_pointer = typename std::pointer_traits<VoidPointer>::template rebind<const BinarySearchTreeNode>;
  // clang-format on

  explicit BinarySearchTreeNode(key_type&& key, pointer left = nullptr,
                                pointer right = nullptr, pointer parent = nullptr)
      : key_(std::forward<Key>(key)), left_(left), right_(right), parent_(parent) {
  }

  key_type key_;
  pointer left_{nullptr};
  pointer right_{nullptr};
  pointer parent_{nullptr};
};
}  // namespace algo::tree