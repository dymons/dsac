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

    [[nodiscard]] int Depth() const {
      const int max_left = left == nullptr ? 0 : left->Depth();
      const int max_right = right == nullptr ? 0 : right->Depth();
      return 1 + std::max(max_left, max_right);
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

  void BalancingSubtree(Node* k) {
    Node* u;
    while (k->parent->color == Color::Red) {
      if (k->parent == k->parent->parent->right) {
        u = k->parent->parent->left;  // uncle
        if (u && u->color == Color::Red) {
          // case 3.1
          u->color = Color::Black;
          k->parent->color = Color::Black;
          k->parent->parent->color = Color::Red;
          k = k->parent->parent;
        } else {
          if (k == k->parent->left) {
            // case 3.2.2
            k = k->parent;
            SmallRightRotation(k);
          }
          // case 3.2.1
          k->parent->color = Color::Black;
          k->parent->parent->color = Color::Red;
          SmallLeftRotation(k->parent->parent);
        }
      } else {
        u = k->parent->parent->right;  // uncle

        if (u && u->color == Color::Red) {
          // mirror case 3.1
          u->color = Color::Black;
          k->parent->color = Color::Black;
          k->parent->parent->color = Color::Red;
          k = k->parent->parent;
        } else {
          if (k == k->parent->right) {
            // mirror case 3.2.2
            k = k->parent;
            SmallLeftRotation(k);
          }
          // mirror case 3.2.1
          k->parent->color = Color::Black;
          k->parent->parent->color = Color::Red;
          SmallRightRotation(k->parent->parent);
        }
      }
      if (k == root_) {
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
    while (child != nullptr) {
      parent = child;
      child = new_node->key < child->key ? child->left : child->right;
    }

    new_node->parent = parent;
    if (parent == nullptr) {
      new_node->color = Color::Black;
      root_ = new_node;
      return;
    } else if (new_node->key < parent->key) {
      parent->left = new_node;
    } else {
      parent->right = new_node;
    }

    const bool bIsGrandparentNull = new_node->parent->parent == nullptr;
    if (bIsGrandparentNull) {
      return;
    }

    BalancingSubtree(new_node);
  }

  [[nodiscard]] int Depth() const {
    return root_ == nullptr ? -1 : root_->Depth() - 1;
  }

  void Visit(Visitor visitor) const {
    VisitImpl(root_, visitor);
  }
};
}  // namespace algo::tree