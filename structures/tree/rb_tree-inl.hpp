#ifndef STRUCTURES_RB_TREE_H_
#error This file may only be included from rb_tree.hpp
#endif

namespace algo::tree {
template <typename T>
int RBTree<T>::Node::MaxDepth() const {
  const int max_left = left == nullptr ? 0 : left->MaxDepth();
  const int max_right = right == nullptr ? 0 : right->MaxDepth();
  return 1 + std::max(max_left, max_right);
}
template <typename T>
int RBTree<T>::Node::MinDepth() const {
  const int min_left = left == nullptr ? 0 : left->MinDepth();
  const int min_right = right == nullptr ? 0 : right->MinDepth();
  return 1 + std::min(min_left, min_right);
}

template <typename T>
void RBTree<T>::Node::Recolor() noexcept {
  color = color == Color::Red ? Color::Black : Color::Red;
}

template <typename T>
typename RBTree<T>::Node* RBTree<T>::Node::GetParent() const noexcept {
  return parent;
}

template <typename T>
typename RBTree<T>::Node* RBTree<T>::Node::GetGrandparent() const noexcept {
  return parent != nullptr ? parent->parent : nullptr;
}

template <typename T>
typename RBTree<T>::Node* RBTree<T>::Node::GetUncle() const noexcept {
  Node* const grandparent = GetGrandparent();
  return grandparent == nullptr        ? nullptr
         : parent == grandparent->left ? grandparent->right
                                       : grandparent->left;
}

template <typename T>
bool RBTree<T>::Node::IsColor(Color other_color) const noexcept {
  return color == other_color;
}

template <typename T>
bool RBTree<T>::Node::Contains(T search_key) const {
  if (key == search_key) {
    return true;
  }

  return (left && left->Contains(search_key)) || (right && right->Contains(search_key));
}

template <typename T>
void RBTree<T>::Node::Destroy() {
  if (left != nullptr) {
    left->Destroy();
    delete left;
  }

  if (right != nullptr) {
    right->Destroy();
    delete right;
  }
}

template <typename T>
void RBTree<T>::SmallLeftRotation(Node* x) {
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

template <typename T>
void RBTree<T>::SmallRightRotation(Node* x) {
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

template <typename T>
void RBTree<T>::BalancingSubtree(Node* subtree) {
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

template <typename T>
void RBTree<T>::VisitImpl(Node* root, Visitor visitor) const {
  if (root != nullptr) {
    VisitImpl(root->left, visitor);
    visitor(root->key);
    VisitImpl(root->right, visitor);
  }
}

template <typename T>
RBTree<T>::~RBTree() {
  if (root_ != nullptr) {
    root_->Destroy();
    delete root_;
  }
}

template <typename T>
void RBTree<T>::Insert(T key) {
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

template <typename T>
int RBTree<T>::MaxDepth() const {
  return root_ == nullptr ? -1 : root_->MaxDepth() - 1;
}

template <typename T>
int RBTree<T>::MinDepth() const {
  return root_ == nullptr ? -1 : root_->MinDepth() - 1;
}

template <typename T>
void RBTree<T>::Visit(Visitor visitor) const {
  VisitImpl(root_, visitor);
}

template <typename T>
bool RBTree<T>::Contains(T key) const {
  return root_ && root_->Contains(key);
}
}  // namespace algo::tree