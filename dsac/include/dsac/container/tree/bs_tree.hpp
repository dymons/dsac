#pragma once

namespace dsac::tree {

template <typename BinarySearchTreeNode>
class BinarySearchTreeIterator final
  : std::iterator<std::bidirectional_iterator_tag, typename BinarySearchTreeNode::pointer> {
  using key_type           = typename BinarySearchTreeNode::key_type;
  using node_pointer       = typename BinarySearchTreeNode::pointer;
  using const_node_pointer = typename BinarySearchTreeNode::const_pointer;

  template <typename, typename, typename>
  friend class BinarySearchTree;

public:
  using iterator_category = const std::bidirectional_iterator_tag;
  using value_type        = typename BinarySearchTreeNode::value_type;
  using const_reference   = typename BinarySearchTreeNode::const_reference;

private:
  bool IsLeftChild(node_pointer node) const noexcept
  {
    return node == node->parent_->left_;
  }

  node_pointer FindMinNode(node_pointer node) const noexcept
  {
    while (node != nullptr && node->left_ != nullptr) {
      node = node->left_;
    }
    return node;
  }

  node_pointer FindMaxNode(node_pointer node) const noexcept
  {
    while (node != nullptr && node->right_ != nullptr) {
      node = node->right_;
    }
    return node;
  }

  node_pointer FindNextNode(node_pointer node) const
  {
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
  static BinarySearchTreeIterator MakeInvalid()
  {
    return {};
  }

  const_reference operator*() const
  {
    return current_node_->key_;
  }

  BinarySearchTreeIterator& operator++()
  {
    current_node_ = FindNextNode(current_node_);
    return *this;
  }

  BinarySearchTreeIterator operator++(int)
  {
    auto latest_iterator = *this;
    ++*this;
    return latest_iterator;
  }

  BinarySearchTreeIterator operator--()    = delete;
  BinarySearchTreeIterator operator--(int) = delete;

  friend bool operator==(const BinarySearchTreeIterator& lhs, const BinarySearchTreeIterator& rhs)
  {
    return lhs.current_node_ == rhs.current_node_;
  }

  friend bool operator!=(const BinarySearchTreeIterator& lhs, const BinarySearchTreeIterator& rhs)
  {
    return !(lhs == rhs);
  }

  BinarySearchTreeIterator()                                = default;
  BinarySearchTreeIterator(const BinarySearchTreeIterator&) = default;
  BinarySearchTreeIterator(BinarySearchTreeIterator&&)      = default;
  BinarySearchTreeIterator& operator=(const BinarySearchTreeIterator&) = default;
  BinarySearchTreeIterator& operator=(BinarySearchTreeIterator&&) = default;
  ~BinarySearchTreeIterator()                                     = default;

private:
  explicit BinarySearchTreeIterator(node_pointer node)
    : current_node_(node)
  {
  }
};

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

  template <typename T>
  explicit BinarySearchTreeNode(T&& key, pointer left = nullptr, pointer right = nullptr, pointer parent = nullptr)
    : key_(std::forward<T>(key))
    , left_(left)
    , right_(right)
    , parent_(parent)
  {
  }

  key_type key_;
  pointer  left_{nullptr};
  pointer  right_{nullptr};
  pointer  parent_{nullptr};
};

template <typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
class BinarySearchTree final {
  using AllocatorTraits = std::allocator_traits<Allocator>;

public:
  using key_type        = Key;
  using value_type      = Key;
  using size_type       = typename AllocatorTraits::size_type;
  using difference_type = typename AllocatorTraits::difference_type;
  using key_compare     = Compare;
  using value_compare   = Compare;
  using allocator_type  = Allocator;
  using reference       = value_type&;
  using const_reference = const value_type&;
  using pointer         = typename AllocatorTraits::pointer;
  using const_pointer   = typename AllocatorTraits::const_pointer;

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
  using iterator       = BinarySearchTreeIterator<node>;
  using const_iterator = BinarySearchTreeIterator<const_node>;

  explicit BinarySearchTree(Compare comp = Compare(), Allocator alloc = Allocator())
    : compare_(std::move(comp))
    , allocator_(node_allocator(std::move(alloc)))
    , size_(0)
    , root_(nullptr){};

  ~BinarySearchTree()
  {
    Clear();
  }

  iterator Begin() noexcept
  {
    return MakeIterator(GetLeftmostNode(root_));
  }

  const_iterator Begin() const noexcept
  {
    return MakeIterator(GetLeftmostNode(root_));
  }

  const_iterator CBegin() const noexcept
  {
    return MakeIterator(GetLeftmostNode(root_));
  }

  iterator End() noexcept
  {
    return iterator::MakeInvalid();
  }

  const_iterator End() const noexcept
  {
    return const_iterator::MakeInvalid();
  }

  const_iterator CEnd() const noexcept
  {
    return const_iterator::MakeInvalid();
  }

  std::pair<iterator, bool> Insert(value_type&& value)
  {
    return InsertUnique(std::move(value));
  }

  std::pair<iterator, bool> Insert(value_type const& value)
  {
    return InsertUnique(value);
  }

  template <typename... Args>
  std::pair<iterator, bool> Emplace(Args... args)
  {
    return EmplaceUnique(std::forward<Args>(args)...);
  }

  inline bool IsEmpty() const noexcept
  {
    return Size() == 0;
  }

  inline std::size_t Size() const noexcept
  {
    return size_;
  }

  node_allocator GetAllocator() const
  {
    return allocator_;
  }

  void Clear()
  {
    DestroySubtree(root_);
  }

  const_iterator Find(key_type const& key) const
  {
    return FindUnique(key);
  }

  bool Erase(key_type const& key)
  {
    return EraseUnique(key);
  }

private:
  [[gnu::always_inline]] iterator MakeIterator(node_pointer node) noexcept
  {
    return iterator(node);
  }

  [[gnu::always_inline]] const_iterator MakeIterator(const_node_pointer node) const noexcept
  {
    return const_iterator(const_cast<node_pointer>(node));
  }

  template <typename... Args>
  node_pointer ConstructNode(Args&&... args)
  {
    node_pointer new_node = node_traits::allocate(allocator_, 1U);
    try {
      node_traits::construct(allocator_, new_node, std::forward<Args>(args)...);
    }
    catch (...) {
      node_traits::deallocate(allocator_, new_node, 1U);
      throw;
    }
    return new_node;
  }

  [[gnu::always_inline]] void DestroyNode(node_pointer& node)
  {
    node_traits::destroy(allocator_, node);
    node_traits::deallocate(allocator_, node, 1U);
    node = nullptr;
  }

  std::pair<iterator, bool> InsertUnique(value_type&& value)
  {
    return InsertUniqueNode(ConstructNode(std::move(value)));
  }

  template <typename... Args>
  std::pair<iterator, bool> EmplaceUnique(Args... args)
  {
    return InsertUniqueNode(ConstructNode(std::forward<Args>(args)...));
  }

  std::pair<iterator, bool> InsertUnique(value_type const& value)
  {
    return InsertUniqueNode(ConstructNode(value));
  }

  std::pair<iterator, bool> InsertUniqueNode(node_pointer inserted_node)
  {
    if (root_ == nullptr) {
      root_ = inserted_node;
      size_++;
      return std::make_pair(MakeIterator(root_), true);
    }

    node_pointer parent        = root_;
    node_pointer parent_before = root_;
    while (parent != nullptr) {
      parent_before = parent;
      if (compare_(inserted_node->key_, parent->key_)) {
        parent = parent->left_;
      }
      else if (compare_(parent->key_, inserted_node->key_)) {
        parent = parent->right_;
      }
      else {
        return std::make_pair(MakeIterator(parent), false);
      }
    }

    if (compare_(inserted_node->key_, parent_before->key_)) {
      parent_before->left_ = inserted_node;
    }
    else {
      parent_before->right_ = inserted_node;
    }

    inserted_node->parent_ = parent_before;
    size_++;

    return std::make_pair(MakeIterator(inserted_node), true);
  }

  [[gnu::always_inline]] node_pointer GetLeftmostNode(node_pointer leftmost) noexcept
  {
    while (leftmost && leftmost->left_) {
      leftmost = leftmost->left_;
    }
    return leftmost;
  }

  [[gnu::always_inline]] const_node_pointer GetLeftmostNode(const_node_pointer leftmost) const noexcept
  {
    while (leftmost && leftmost->left_) {
      leftmost = leftmost->left_;
    }
    return leftmost;
  }

  void DestroySubtree(node_pointer& root)
  {
    node_pointer next_to_destroy = nullptr;
    for (; root != nullptr; root = next_to_destroy) {
      if (root->left_ == nullptr) {
        next_to_destroy = root->right_;
        DestroyNode(root);
      }
      else {
        next_to_destroy         = root->left_;
        root->left_             = next_to_destroy->right_;
        next_to_destroy->right_ = root;
      }
    }

    size_ = 0;
  }

  iterator FindUnique(key_type const& key)
  {
    node_pointer root = root_;
    while (root != nullptr) {
      if (compare_(key, root->key_)) {
        root = root->left_;
      }
      else if (compare_(root->key_, key)) {
        root = root->right_;
      }
      else {
        return MakeIterator(root);
      }
    }

    return End();
  }

  const_iterator FindUnique(key_type const& key) const
  {
    const_node_pointer root = root_;
    while (root != nullptr) {
      if (compare_(key, root->key_)) {
        root = root->left_;
      }
      else if (compare_(root->key_, key)) {
        root = root->right_;
      }
      else {
        return MakeIterator(root);
      }
    }

    return CEnd();
  }

  bool EraseUnique(key_type const& key)
  {
    if (iterator it = FindUnique(key); it != End()) {
      bool const has_left_node  = it.current_node_->left_ != nullptr;
      bool const has_right_node = it.current_node_->right_ != nullptr;
      if (!has_left_node && !has_right_node) {
        if (it.current_node_ == root_) {
          DestroyNode(root_);
        }
        else {
          if (it.current_node_->parent_->left_ == it.current_node_) {
            it.current_node_->parent_->left_ = nullptr;
          }
          else {
            it.current_node_->parent_->right_ = nullptr;
          }

          DestroyNode(it.current_node_);
        }
      }
      else if (has_left_node && has_right_node) {
        node_pointer current_leftmost = GetLeftmostNode(it.current_node_->right_);

        it.current_node_->key_ = std::move(current_leftmost->key_);
        if (current_leftmost->parent_->left_ == current_leftmost) {
          current_leftmost->parent_->left_ = current_leftmost->right_;
        }
        else {
          current_leftmost->parent_->right_ = current_leftmost->right_;
        }

        DestroyNode(current_leftmost);
      }
      else {
        node_pointer child_node = has_left_node ? it.current_node_->left_ : it.current_node_->right_;
        if (it.current_node_ == root_) {
          root_ = child_node;
        }
        else {
          if (it.current_node_ == it.current_node_->parent_->left_) {
            it.current_node_->parent_->left_ = child_node;
          }
          else {
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
  node_allocator    allocator_;
  node_pointer      root_;
  size_t            size_;
};

template <typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
auto begin(BinarySearchTree<Key, Compare, Allocator> const& bst)
{  // NOLINT
  return bst.Begin();
}
template <typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
auto end(BinarySearchTree<Key, Compare, Allocator> const& bst)
{  // NOLINT
  return bst.End();
}
}  // namespace dsac::tree