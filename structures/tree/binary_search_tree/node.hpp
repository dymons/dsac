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

  explicit BinarySearchTreeNode(const_reference key, pointer left = nullptr,
                                pointer right = nullptr, pointer parent = nullptr)
      : key_(key), left_(left), right_(right), parent_(parent) {
  }

  const_reference key_;
  pointer left_{nullptr};
  pointer right_{nullptr};
  pointer parent_{nullptr};
};

template <typename NodeAllocator>
class BinarySearchTreeNodeDestructor final {
 public:
  using allocator_type = NodeAllocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using value_type = typename allocator_traits::value_type;
  using pointer = typename std::allocator_traits<allocator_type>::pointer;

 private:
  allocator_type& node_allocator_;

 public:
  bool value_constructed_;
  explicit BinarySearchTreeNodeDestructor(allocator_type& node_allocator) noexcept
      : node_allocator_(node_allocator), value_constructed_(false) {
  }

  void operator()(pointer p) noexcept {
    if (value_constructed_) {
      allocator_traits::destroy(node_allocator_, std::addressof(p->key_));
    }
    if (p) {
      allocator_traits::deallocate(node_allocator_, p, 1);
    }
  }
};
}  // namespace algo::tree