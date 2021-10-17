#pragma once

#include <vector>
#include <functional>
#include <iostream>
namespace algo::sort {
namespace detail {
template <typename T>
class BinaryTree final {
 public:
  using Visitor = std::function<void(T)>;

  BinaryTree() = default;
  BinaryTree(const BinaryTree&) = default;
  BinaryTree(BinaryTree&&) = default;
  BinaryTree& operator=(const BinaryTree&) = default;
  BinaryTree& operator=(BinaryTree&&) = default;
  ~BinaryTree() {
    Clear(root_);
  }

  void Insert(T data) {
    Node* new_node = new Node{};
    new_node->data = data;

    if (root_ == nullptr) {
      root_ = new_node;
      return;
    }

    InsertImpl(root_, new_node);
  }

  void Visit(Visitor visitor) {
    if (root_ != nullptr) {
      VisitImpl(root_, visitor);
    }
  }

 private:
  struct Node {
    Node* left = nullptr;
    Node* right = nullptr;
    T data;
  };

  Node* root_ = nullptr;

  void InsertImpl(Node* root, Node* added) {
    const bool is_added_less = added->data < root->data;
    if (is_added_less) {
      if (root->left == nullptr) {
        root->left = added;
      } else {
        InsertImpl(root->left, added);
      }
    } else {
      if (root->right == nullptr) {
        root->right = added;
      } else {
        InsertImpl(root->right, added);
      }
    }
  }

  void VisitImpl(Node* root, Visitor visitor) {
    if (root == nullptr) {
      return;
    }

    VisitImpl(root->left, visitor);
    visitor(root->data);
    VisitImpl(root->right, visitor);
  }

  void Clear(Node* root) {
    if (root != nullptr) {
      Clear(root->left);
      Clear(root->right);
      delete root;
    }
  }
};
}  // namespace detail

template <typename T>
void BinaryTreeSort(std::vector<T>& arr) {
  detail::BinaryTree<T> tree;
  for (const T& element : arr) {
    tree.Insert(element);
  }

  int i = 0;
  tree.Visit([&i, &arr](T data) mutable {
    arr[i] = data;
    i = i + 1;
  });
}
}  // namespace algo::sort