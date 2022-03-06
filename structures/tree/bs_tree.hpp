#pragma once

#include <functional>
#include <memory>
#include <iterator>

namespace algo::tree {
template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<Key>>
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

 public:
  template <typename BinarySearchTreeNode>
  class BinarySearchTreeIterator final
      : std::iterator<std::bidirectional_iterator_tag,
                      typename BinarySearchTreeNode::pointer> {
    using key_type = typename BinarySearchTreeNode::key_type;
    using node_pointer = typename BinarySearchTreeNode::pointer;

    template <typename, typename, typename>
    friend class BinarySearchTree;

   public:
    using iterator_category = const std::bidirectional_iterator_tag;
    using value_type = typename BinarySearchTreeNode::value_type;
    using const_reference = typename BinarySearchTreeNode::const_reference;

   private:
    bool IsLeftChild(node_pointer node) const noexcept {
      return (node == static_cast<node_pointer>(node->parent_->left_));
    }

    node_pointer FindMinNode(node_pointer node) noexcept {
      while (node != nullptr && node->left_ != nullptr) {
        node = node->left_;
      }
      return node;
    }

    node_pointer FindMaxNode(node_pointer node) noexcept {
      while (node != nullptr && node->right_ != nullptr) {
        node = node->right_;
      }
      return node;
    }

    node_pointer FindNextNode(node_pointer node) {
      if (node != nullptr && node->right_ != nullptr) {
        return FindMinNode(node->right_);
      }

      while (!IsLeftChild(node)) {
        node = node->parent_;
      }

      return node->parent_;
    }

    node_pointer current_node_;

   public:
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

    BinarySearchTreeIterator() = delete;
    BinarySearchTreeIterator(const BinarySearchTreeIterator&) = default;
    BinarySearchTreeIterator(BinarySearchTreeIterator&&) = default;
    BinarySearchTreeIterator& operator=(const BinarySearchTreeIterator&) = default;
    BinarySearchTreeIterator& operator=(BinarySearchTreeIterator&&) = default;
    ~BinarySearchTreeIterator() = default;

   private:
    explicit BinarySearchTreeIterator(const node_pointer& node) : current_node_(node) {
    }
  };

 public:
  using iterator = BinarySearchTreeIterator<node>;
  using const_iterator = BinarySearchTreeIterator<const_node>;

  explicit BinarySearchTree(const Compare& comp = Compare(),
                            const Allocator& alloc = Allocator())
      : compare_(comp),
        allocator_(node_allocator(alloc)),
        size_(0),
        header_(MakeNodeHolder(value_type{}).release()) {
    this->header_->left_ = this->header_;
    this->header_->right_ = this->header_;
  };

  ~BinarySearchTree() = default;

  iterator begin() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_->left_);
  }

  const_iterator begin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_->left_);
  }

  const_iterator cbegin() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_->left_);
  }

  iterator end() noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_);
  }

  const_iterator end() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_);
  }

  const_iterator cend() const noexcept {  // NOLINT(readability-identifier-naming)
    return MakeIteratorBy(header_);
  }

  inline bool IsEmpty() const noexcept {  // NOLINT(readability-identifier-naming)
    return Size() == 0;
  }

  inline size_t Size() const noexcept {  // NOLINT(readability-identifier-naming)
    return size_;
  }

 private:
  const key_compare compare_;
  node_allocator allocator_;
  size_t size_;
  node_pointer header_;

  iterator MakeIteratorBy(node_pointer& node) noexcept {
    return iterator(node);
  }

  const_iterator MakeIteratorBy(const node_pointer& node) const noexcept {
    return const_iterator(node);
  }

  node_holder MakeNodeHolder(value_type&& value) {
    node_holder nh(allocator_.allocate(1), node_destructor(allocator_));
    node_traits::construct(allocator_, nh.get(), std::forward<value_type>(value));
    nh.get_deleter().value_constructed_ = true;
    return nh;
  }
};
}  // namespace algo::tree