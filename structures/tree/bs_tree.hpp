#pragma once

#include <memory>

namespace algo::tree {

template <typename Key, typename Compare, typename Allocator = std::allocator<Key>>
class BinarySearchTree final {
  using AllocatorTraits = std::allocator_traits<Allocator>;

  template <typename VoidPointer>
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
    using allocator_type = NodeAllocator;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using value_type = typename allocator_traits::value_type;

   public:
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
  };

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
  using node = BinarySearchTreeNode<typename AllocatorTraits::void_pointer>;
  using const_node = const BinarySearchTreeNode<typename AllocatorTraits::void_pointer>;
  using node_allocator = typename AllocatorTraits::template rebind_alloc<node>;
  using node_traits = std::allocator_traits<node_allocator>;
  using node_pointer = typename node_traits::pointer;
  using const_node_pointer = typename node_traits::const_pointer;
  using node_destructor = BinarySearchTreeNodeDestructor<node_allocator>;
  using node_holder = std::unique_ptr<node, node_destructor>;
};
}  // namespace algo::tree