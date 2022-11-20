#pragma once

#include "../../../../../../../../../../usr/include/c++/11/functional"

namespace dsac::tree {
template <typename T>
class AVLTree final {
public:
  using Visitor = std::function<void(T)>;

private:
  class Node final {
    const T key_;
    Node*   left_;
    Node*   right_;
    int     height_;

  public:
    explicit Node(T key);

    Node(const Node&)            = delete;
    Node(Node&&)                 = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&)      = delete;

    void                 SetLeftChild(Node* child) noexcept;
    void                 SetRightChild(Node* child) noexcept;
    void                 SetHeight(int height) noexcept;
    [[nodiscard]] Node*& GetLeftChild() & noexcept;
    [[nodiscard]] Node*& GetRightChild() & noexcept;
    [[nodiscard]] int    GetHeight() noexcept;
    void                 Destroy();

    [[nodiscard]] const T& GetKey() const noexcept;
    [[nodiscard]] bool     Contains(T key) const;
  };

  Node* root_{nullptr};

  void SmallLeftRotation(Node*& subtree) const;
  void LargeLeftRotation(Node*& subtree) const;
  void SmallRightRotation(Node*& subtree) const;
  void LargeRightRotation(Node*& subtree) const;

  template <typename Comp>
  void BalancingSubtree(Node*& subtree, T destination_key, T source_key, Comp comp) const;

  Node* DeleteMinChild(Node* subtree);
  Node* FindMinChild(Node* subtree) const;

  [[nodiscard]] int   GetMaxHeight(Node* left_subtree, Node* right_subtree) const;
  [[nodiscard]] bool  InsertImpl(Node*& root, Node* added) const;
  [[nodiscard]] Node* DeleteImpl(Node*& root, T deleted_key);
  void                VisitImpl(Node* root, Visitor visitor) const;

public:
  AVLTree()                          = default;
  AVLTree(const AVLTree&)            = delete;
  AVLTree(AVLTree&&)                 = delete;
  AVLTree& operator=(const AVLTree&) = delete;
  AVLTree& operator=(AVLTree&&)      = delete;
  ~AVLTree();

  bool Insert(T added_key);
  void Delete(T deleted_key);

  [[nodiscard]] bool Contains(T key) const;
  [[nodiscard]] int  Depth() const;

  void Visit(Visitor visitor) const;
};
}  // namespace dsac::tree

#define STRUCTURES_AVL_TREE_H_
#include "avl_tree-inl.hpp"
#undef STRUCTURES_AVL_TREE_H_