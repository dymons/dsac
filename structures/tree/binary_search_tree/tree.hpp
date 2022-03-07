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
  using node_destructor = BinarySearchTreeNodeDestructor<node_allocator>;
  using node_holder = std::unique_ptr<node, node_destructor>;
  // clang-format on

 public:
  using iterator = BinarySearchTreeIterator<node>;
  using const_iterator = BinarySearchTreeIterator<const_node>;

  explicit BinarySearchTree(const Compare& comp = Compare(),
                            const Allocator& alloc = Allocator())
      : compare_(comp),
        allocator_(node_allocator(alloc)),
        size_(0),
        root_(MakeNodeHolder(value_type{}).release()) {
    root_->left_ = root_;
    root_->right_ = root_;
  };

  ~BinarySearchTree() {
    node_holder nh(root_, node_destructor(allocator_));
  }

  iterator begin() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_->left_);
  }

  const_iterator begin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_->left_);
  }

  const_iterator cbegin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_->left_);
  }

  iterator end() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_);
  }

  const_iterator end() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_);
  }

  const_iterator cend() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(root_);
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
  const key_compare compare_;
  node_allocator allocator_;
  size_t size_;
  node_pointer root_;

  iterator MakeIteratorBy(node_pointer& node) noexcept {
    return iterator(node);
  }

  const_iterator MakeIteratorBy(const node_pointer& node) const noexcept {
    return const_iterator(node);
  }

  template <typename T>
  node_holder MakeNodeHolder(T&& value) {
    static_assert(std::is_same_v<T, typename node_allocator::value_type::value_type>, "");
    node_holder nh(allocator_.allocate(1), node_destructor(allocator_));
    node_traits::construct(allocator_, nh.get(), std::forward<T>(value));
    return nh;
  }
};
}  // namespace algo::tree