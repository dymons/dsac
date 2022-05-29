#ifndef STRUCTURES_AVL_TREE_H_
#error This file may only be included from avl_tree.hpp
#endif

#include "../../../../../../../../../../usr/include/c++/11/cassert"

namespace dsac::tree {
template <typename T>
AVLTree<T>::Node::Node(T key)
  : key_(key)
  , left_(nullptr)
  , right_(nullptr)
  , height_(0) {
}

template <typename T>
void AVLTree<T>::Node::SetLeftChild(Node* child) noexcept {
  left_ = child;
}

template <typename T>
void AVLTree<T>::Node::SetRightChild(Node* child) noexcept {
  right_ = child;
}

template <typename T>
void AVLTree<T>::Node::SetHeight(int height) noexcept {
  height_ = height;
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::GetLeftChild() & noexcept {
  return left_;
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::GetRightChild() & noexcept {
  return right_;
}

template <typename T>
const T& AVLTree<T>::Node::GetKey() const noexcept {
  return key_;
}

template <typename T>
int AVLTree<T>::Node::GetHeight() noexcept {
  return height_;
}

template <typename T>
bool AVLTree<T>::Node::Contains(T key) const {
  if (key == key_) {
    return true;
  }

  return (left_ != nullptr && left_->Contains(key)) || (right_ != nullptr && right_->Contains(key));
}

template <typename T>
void AVLTree<T>::Node::Destroy() {
  // TODO (Dmitry Emelyanov) Вынести удаление объекта в PostOrder на уровень
  // дерева
  // TODO (Dmitry Emelyanov) Использовать аллокаторы памяти
  if (left_ != nullptr) {
    left_->Destroy();
    delete left_;
  }

  if (right_ != nullptr) {
    right_->Destroy();
    delete right_;
  }
}

template <typename T>
void AVLTree<T>::SmallLeftRotation(Node*& subtree) const {
  Node* const heavy_right = subtree->GetRightChild();

  subtree->SetRightChild(heavy_right->GetLeftChild());
  heavy_right->SetLeftChild(subtree);

  subtree->SetHeight(GetMaxHeight(subtree->GetLeftChild(), subtree->GetRightChild()));
  heavy_right->SetHeight(GetMaxHeight(subtree, heavy_right->GetRightChild()));

  subtree = heavy_right;
}

template <typename T>
void AVLTree<T>::SmallRightRotation(Node*& subtree) const {
  Node* const heavy_left = subtree->GetLeftChild();

  subtree->SetLeftChild(heavy_left->GetRightChild());
  heavy_left->SetRightChild(subtree);

  subtree->SetHeight(GetMaxHeight(subtree->GetLeftChild(), subtree->GetRightChild()));
  heavy_left->SetHeight(GetMaxHeight(heavy_left->GetLeftChild(), subtree));

  subtree = heavy_left;
}

template <typename T>
void AVLTree<T>::LargeLeftRotation(Node*& subtree) const {
  SmallRightRotation(subtree->GetRightChild());
  SmallLeftRotation(subtree);
}

template <typename T>
void AVLTree<T>::LargeRightRotation(Node*& subtree) const {
  SmallLeftRotation(subtree->GetLeftChild());
  SmallRightRotation(subtree);
}

template <typename T>
template <typename Comp>
void AVLTree<T>::BalancingSubtree(Node*& subtree, T destination_key, T source_key, Comp comp) const {
  const int left_depth  = GetMaxHeight(subtree->GetLeftChild(), nullptr);
  const int right_depth = GetMaxHeight(nullptr, subtree->GetRightChild());

  const int balance_factor = left_depth - right_depth;
  if (balance_factor == -2) {
    if (comp(destination_key, source_key)) {
      LargeLeftRotation(subtree);
    } else {
      SmallLeftRotation(subtree);
    }
  } else if (balance_factor == 2) {
    if (comp(destination_key, source_key)) {
      LargeRightRotation(subtree);
    } else {
      SmallRightRotation(subtree);
    }
  }

  subtree->SetHeight(GetMaxHeight(subtree->GetLeftChild(), subtree->GetRightChild()));
}

template <typename T>
bool AVLTree<T>::InsertImpl(Node*& root, Node* added) const {
  if (root == nullptr) {
    root = added;
    return true;
  } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
    if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
      T const& dest_key   = added->GetKey();
      T const& source_key = root->GetLeftChild()->GetKey();
      BalancingSubtree(root, dest_key, source_key, std::greater{});
      return true;
    }
  } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
    if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
      T const& dest_key   = added->GetKey();
      T const& source_key = root->GetRightChild()->GetKey();
      BalancingSubtree(root, dest_key, source_key, std::less{});
      return true;
    }
  }

  return false;
}

template <typename T>
int AVLTree<T>::GetMaxHeight(Node* left_subtree, Node* right_subtree) const {
  const int left_height  = left_subtree == nullptr ? 0 : left_subtree->GetHeight() + 1;
  const int right_height = right_subtree == nullptr ? 0 : right_subtree->GetHeight() + 1;
  return std::max(left_height, right_height);
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::FindMinChild(Node* subtree) const {
  Node* const child = subtree->GetLeftChild();
  return child != nullptr ? FindMinChild(child) : subtree;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::DeleteMinChild(Node* subtree) {
  Node* left_child = subtree->GetLeftChild();
  if (left_child == nullptr) {
    return subtree->GetRightChild();
  }

  subtree->SetLeftChild(DeleteMinChild(left_child));
  return subtree;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::DeleteImpl(Node*& root, T deleted_key) {
  if (root == nullptr) {
    return nullptr;
  } else if (const bool is_less = deleted_key < root->GetKey(); is_less) {
    root->SetLeftChild(DeleteImpl(root->GetLeftChild(), deleted_key));
    BalancingSubtree(root, deleted_key, root->GetKey(), std::greater{});
    return root;
  } else if (const bool is_more = deleted_key > root->GetKey(); is_more) {
    root->SetRightChild(DeleteImpl(root->GetRightChild(), deleted_key));
    BalancingSubtree(root, deleted_key, root->GetKey(), std::less{});
    return root;
  }

  Node* left_child  = root->GetLeftChild();
  Node* right_child = root->GetRightChild();
  delete root;

  if (right_child == nullptr) {
    return left_child;
  }

  Node* const min_child = FindMinChild(right_child);
  min_child->SetRightChild(DeleteMinChild(right_child));
  min_child->SetLeftChild(left_child);
  return min_child;
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
AVLTree<T>::~AVLTree() {
  if (root_ != nullptr) {
    root_->Destroy();
    delete root_;
  }
}

template <typename T>
bool AVLTree<T>::Insert(T added_key) {
  if (const bool exist = Contains(added_key); !exist) {
    return InsertImpl(root_, new Node{added_key});
  }

  return false;
}
template <typename T>
void AVLTree<T>::Delete(T deleted_key) {
  root_ = DeleteImpl(root_, deleted_key);
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
  return root_ == nullptr ? -1 : root_->GetHeight();
}
}  // namespace dsac::tree