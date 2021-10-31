#ifndef STRUCTURES_AVL_TREE_H_
#error This file may only be included from avl_tree.hpp
#endif

#include <cassert>

namespace algo::tree {
template <typename T>
AVLTree<T>::Node::Node(T key) : key_(key), left_(nullptr), right_(nullptr) {
}

template <typename T>
void AVLTree<T>::Node::SetLeftChild(Node* node) noexcept {
  left_ = node;
}

template <typename T>
void AVLTree<T>::Node::SetRightChild(Node* node) noexcept {
  right_ = node;
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::GetLeftChild() noexcept {
  return left_;
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::GetRightChild() noexcept {
  return right_;
}

template <typename T>
const T& AVLTree<T>::Node::GetKey() const noexcept {
  return key_;
}

template <typename T>
bool AVLTree<T>::Node::Contains(T key) const {
  if (key == key_) {
    return true;
  }

  return (left_ != nullptr && left_->Contains(key)) ||
         (right_ != nullptr && right_->Contains(key));
}

template <typename T>
void AVLTree<T>::Node::Destroy() {
  if (left_ != nullptr) {
    left_->Destroy();
    delete left_;
    left_ = nullptr;
  }

  if (right_ != nullptr) {
    right_->Destroy();
    delete right_;
    right_ = nullptr;
  }
}

template <typename T>
void AVLTree<T>::SmallLeftRotation(Node*& root) const {
  Node* const heavy_right = root->GetRightChild();

  root->SetRightChild(heavy_right->GetLeftChild());
  heavy_right->SetLeftChild(root);

  root = heavy_right;
}

template <typename T>
void AVLTree<T>::LargeLeftRotation(Node*& latest_root) const {
  Node* const right_child = latest_root->GetRightChild();
  Node* const new_root = right_child->GetLeftChild();

  right_child->SetLeftChild(new_root->GetRightChild());
  new_root->SetRightChild(right_child);
  latest_root->SetRightChild(new_root->GetLeftChild());
  new_root->SetLeftChild(latest_root);

  latest_root = new_root;
}

template <typename T>
void AVLTree<T>::SmallRightRotation(Node*& root) const {
  Node* const heavy_left = root->GetLeftChild();

  root->SetLeftChild(heavy_left->GetRightChild());
  heavy_left->SetRightChild(root);

  root = heavy_left;
}

template <typename T>
void AVLTree<T>::LargeRightRotation(Node*& latest_root) const {
  Node* const left_child = latest_root->GetLeftChild();
  Node* const new_root = left_child->GetRightChild();

  left_child->SetRightChild(new_root->GetLeftChild());
  new_root->SetLeftChild(left_child);
  latest_root->SetLeftChild(new_root->GetRightChild());
  new_root->SetRightChild(latest_root);

  latest_root = new_root;
}

template <typename T>
void AVLTree<T>::BalancingLeftSubtree(Node*& root, Node* added) const {
  const int left_depth = DepthImpl(root->GetLeftChild());
  const int right_depth = DepthImpl(root->GetRightChild());

  const int balance_factor = std::abs(left_depth - right_depth);
  if (balance_factor == 2) {
    if (added->GetKey() < root->GetLeftChild()->GetKey()) {
      SmallRightRotation(root);
    } else {
      LargeRightRotation(root);
    }
  }
}

template <typename T>
void AVLTree<T>::BalancingRightSubtree(Node*& root, Node* added) const {
  const int left_depth = DepthImpl(root->GetLeftChild());
  const int right_depth = DepthImpl(root->GetRightChild());

  const int balance_factor = std::abs(left_depth - right_depth);
  if (balance_factor == 2) {
    if (added->GetKey() > root->GetRightChild()->GetKey()) {
      SmallLeftRotation(root);
    } else {
      LargeLeftRotation(root);
    }
  }
}

template <typename T>
bool AVLTree<T>::InsertImpl(Node*& root, Node* added) const {
  if (root == nullptr) {
    root = added;
    return true;
  } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
    if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
      BalancingLeftSubtree(root, added);
      return true;
    }
  } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
    if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
      BalancingRightSubtree(root, added);
      return true;
    }
  }

  return false;
}

template <typename T>
void AVLTree<T>::VisitImpl(Node* root, Visitor visitor) const {
  if (root != nullptr) {
    VisitImpl(root->GetLeftChild(), visitor);
    visitor(root->GetKey());
    VisitImpl(root->GetRightChild(), visitor);
  }
}

template <typename T>
int AVLTree<T>::DepthImpl(Node* root) const {
  return root == nullptr ? 0
                         : 1 + std::max(DepthImpl(root->GetLeftChild()),
                                        DepthImpl(root->GetRightChild()));
}

template <typename T>
AVLTree<T>::~AVLTree() {
  if (root_ != nullptr) {
    root_->Destroy();
    delete root_;
    root_ = nullptr;
  }
}

template <typename T>
void AVLTree<T>::Insert(T added_key) {
  if (const bool exist = Contains(added_key); !exist) {
    InsertImpl(root_, new Node{added_key});
  }
}

template <typename T>
void AVLTree<T>::Visit(Visitor visitor) const {
  VisitImpl(root_, visitor);
}

template <typename T>
bool AVLTree<T>::Contains(T key) const {
  return root_ != nullptr && root_->Contains(key);
}

template <typename T>
int AVLTree<T>::Depth() const {
  return DepthImpl(root_) - 1;
}
}  // namespace algo::tree