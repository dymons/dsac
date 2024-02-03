#ifndef STRUCTURES_B_TREE_H_
#error This file may only be included from b_tree.hpp
#endif

#include "../../../../../../../../../../usr/include/c++/11/cassert"

namespace dsac::tree {
template <typename T>
BTree<T>::Node::Node(int t)
  : t_(t) {
}

template <typename T>
void BTree<T>::Node::AddKeyByIndex(std::size_t index, T key) {
  keys_.insert(std::next(keys_.begin(), index), key);
  // Ключи хранятся в отсортированном виде, проверяем инвариант
  // состояния узла на выполнение данного условия.
  assert(std::is_sorted(keys_.begin(), keys_.end()));
}
template <typename T>
T const& BTree<T>::Node::GetKey(std::size_t index) const {
  assert(index < keys_.size());
  return keys_[index];
}

template <typename T>
void BTree<T>::Node::RemoveKey(T key) {
  keys_.erase(std::find(keys_.begin(), keys_.end(), key));
}

template <typename T>
void BTree<T>::Node::AddChild(Node* node) {
  children_.push_back(node);
}

template <typename T>
void BTree<T>::Node::AddChildByIndex(std::size_t index, Node* node) {
  children_.insert(std::next(children_.begin(), index), node);
}

template <typename T>
typename BTree<T>::Node* BTree<T>::Node::GetChild(std::size_t index) const {
  assert(index < children_.size());
  return children_[index];
}

template <typename T>
bool BTree<T>::Node::IsLeaf() const noexcept {
  return children_.empty();
}

template <typename T>
bool BTree<T>::Node::IsKeysFull() const noexcept {
  return keys_.size() == (2 * t_ - 1);
}

template <typename T>
void BTree<T>::Node::AddKey(T key) {
  if (IsLeaf()) {
    AddKeyImpl(key);
  } else {
    const auto upper = std::upper_bound(keys_.begin(), keys_.end(), key);
    int        index = std::distance(keys_.begin(), upper);
    if (Node* const consider_node = GetChild(index); consider_node->IsKeysFull()) {
      SplitChild(index, consider_node);
      if (GetKey(index) < key) {
        index++;
      }
    }

    GetChild(index)->AddKey(key);
  }
}

template <typename T>
void BTree<T>::Node::AddKeyImpl(T key) {
  keys_.insert(std::upper_bound(keys_.begin(), keys_.end(), key), key);
  // Ключи хранятся в отсортированном виде, проверяем инвариант
  // состояния узла на выполнение данного условия.
  assert(std::is_sorted(keys_.begin(), keys_.end()));
}

template <typename T>
void BTree<T>::Node::SplitChild(std::size_t index, Node* root) {
  Node* const new_node = new Node{root->t_};
  for (int j = 0; j < t_ - 1; j++) {
    new_node->AddKey(root->GetKey(j + t_));
  }

  if (!root->IsLeaf()) {
    for (int j = 0; j < t_ - 1; j++) {
      new_node->AddChild(root->GetChild(j + t_));
    }
  }

  AddChildByIndex(index + 1, new_node);
  AddKeyByIndex(index, root->GetKey(t_ - 1));

  for (int j = (2 * t_ - 2); j >= (t_ - 1); --j) {
    root->RemoveKey(root->GetKey(j));
  }
}

template <typename T>
bool BTree<T>::Node::Contains(T key) const {
  const auto        lower = std::lower_bound(keys_.begin(), keys_.end(), key);
  const std::size_t index = std::distance(keys_.begin(), lower);

  if ((index < keys_.size()) && (GetKey(index) == key)) {
    return true;
  } else if (IsLeaf()) {
    return false;
  }

  return GetChild(index)->Contains(key);
}

template <typename T>
void BTree<T>::Node::Destroy() {
  for (Node*& child : children_) {
    child->Destroy();
    delete child;
    child = nullptr;
  }
}

template <typename T>
BTree<T>::BTree(int t)
  : t_(t)
  , root_(nullptr) {
}

template <typename T>
BTree<T>::~BTree() {
  if (root_ != nullptr) {
    root_->Destroy();
    delete root_;
    root_ = nullptr;
  }
}

template <typename T>
void BTree<T>::Insert(T key) {
  if (root_ == nullptr) {
    root_ = new Node{t_};
    root_->AddKey(key);
  } else if (root_->IsKeysFull()) {
    Node* const new_node = new Node{t_};
    new_node->AddChild(root_);
    new_node->SplitChild(0, root_);

    const std::size_t index = new_node->GetKey(0) < key ? 1 : 0;
    new_node->GetChild(index)->AddKey(key);

    root_ = new_node;
  } else {
    root_->AddKey(key);
  }
}

template <typename T>
bool BTree<T>::Contains(T key) const {
  return root_ != nullptr && root_->Contains(key);
}

}  // namespace dsac::tree
