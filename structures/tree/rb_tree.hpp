#pragma once

#include <functional>

namespace algo::tree {
template <typename T>
class RBTree final {
 public:
  using Visitor = std::function<void(T)>;

 private:
  enum class Color : unsigned char { Red, Black };
  struct Node final {
    T key;
    Node* left{nullptr};
    Node* right{nullptr};
    Node* parent{nullptr};
    Color color{Color::Red};

    [[nodiscard]] int MaxDepth() const {
      const int max_left = left == nullptr ? 0 : left->MaxDepth();
      const int max_right = right == nullptr ? 0 : right->MaxDepth();
      return 1 + std::max(max_left, max_right);
    }

    [[nodiscard]] int MinDepth() const {
      const int min_left = left == nullptr ? 0 : left->MinDepth();
      const int min_right = right == nullptr ? 0 : right->MinDepth();
      return 1 + std::min(min_left, min_right);
    }

    void Recolor() noexcept {
      color = color == Color::Red ? Color::Black : Color::Red;
    }

    [[nodiscard, gnu::always_inline]] inline Node* GetParent() const noexcept {
      return parent;
    }

    [[nodiscard, gnu::always_inline]] inline Node* GetGrandparent() const noexcept {
      return parent != nullptr ? parent->parent : nullptr;
    }

    [[nodiscard, gnu::always_inline]] inline Node* GetUncle() const noexcept {
      Node* const grandparent = GetGrandparent();
      return grandparent == nullptr        ? nullptr
             : parent == grandparent->left ? grandparent->right
                                           : grandparent->left;
    }

    [[nodiscard, gnu::always_inline]] inline bool IsColor(
        Color other_color) const noexcept {
      return color == other_color;
    }

    [[nodiscard]] bool Contains(T search_key) const {
      if (key == search_key) {
        return true;
      }

      return (left && left->Contains(search_key)) ||
             (right && right->Contains(search_key));
    }
  };

  void SmallLeftRotation(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
      y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
      root_ = y;
    } else if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
  }

  void SmallRightRotation(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != nullptr) {
      y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
      root_ = y;
    } else if (x == x->parent->right) {
      x->parent->right = y;
    } else {
      x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
  }

  void BalancingSubtree(Node* subtree) {
    while (subtree->GetParent()->IsColor(Color::Red)) {
      if (subtree->GetParent() == subtree->GetGrandparent()->right) {
        if (Node* uncle = subtree->GetUncle(); uncle && uncle->IsColor(Color::Red)) {
          // case 3.1
          uncle->Recolor();
          subtree->GetParent()->Recolor();
          subtree->GetGrandparent()->Recolor();
          subtree = subtree->GetGrandparent();
        } else {
          if (subtree == subtree->GetParent()->left) {
            // case 3.2.2
            subtree = subtree->GetParent();
            SmallRightRotation(subtree);
          }
          // case 3.2.1
          subtree->GetParent()->Recolor();
          subtree->GetGrandparent()->Recolor();
          SmallLeftRotation(subtree->GetGrandparent());
        }
      } else {
        if (Node* uncle = subtree->GetUncle(); uncle && uncle->IsColor(Color::Red)) {
          // mirror case 3.1
          uncle->Recolor();
          subtree->GetParent()->Recolor();
          subtree->GetGrandparent()->Recolor();
          subtree = subtree->GetGrandparent();
        } else {
          if (subtree == subtree->GetParent()->right) {
            // mirror case 3.2.2
            subtree = subtree->GetParent();
            SmallLeftRotation(subtree);
          }
          // mirror case 3.2.1
          subtree->GetParent()->Recolor();
          subtree->GetGrandparent()->Recolor();
          SmallRightRotation(subtree->GetGrandparent());
        }
      }

      if (subtree == root_) {
        break;
      }
    }
    root_->color = Color::Black;
  }

  void VisitImpl(Node* root, Visitor visitor) const {
    if (root != nullptr) {
      VisitImpl(root->left, visitor);
      visitor(root->key);
      VisitImpl(root->right, visitor);
    }
  }

  Node* root_{nullptr};

 public:
  void Insert(T key) {
    Node* new_node = new Node{key};

    Node* parent = nullptr;
    Node* child = root_;
    while (child != nullptr) [[likely]] {
        parent = child;
        child = new_node->key < child->key ? child->left : child->right;
      }

    new_node->parent = parent;
    if (parent == nullptr) [[unlikely]] {
      new_node->color = Color::Black;
      root_ = new_node;
      return;
    } else if (new_node->key < parent->key) {
      parent->left = new_node;
    } else {
      parent->right = new_node;
    }

    const bool bIsGrandparentNull = new_node->parent->parent == nullptr;
    if (bIsGrandparentNull) [[unlikely]] {
      return;
    }

    BalancingSubtree(new_node);
  }

  [[nodiscard]] int MaxDepth() const {
    return root_ == nullptr ? -1 : root_->MaxDepth() - 1;
  }

  [[nodiscard]] int MinDepth() const {
    return root_ == nullptr ? -1 : root_->MinDepth() - 1;
  }

  void Visit(Visitor visitor) const {
    VisitImpl(root_, visitor);
  }

  [[nodiscard]] bool Contains(T key) const {
    return root_ && root_->Contains(key);
  }
};
}  // namespace algo::tree