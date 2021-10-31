#pragma once

#include <functional>

namespace algo::tree {
template <typename T>
class AVLTree final {
 public:
  using Visitor = std::function<void(T)>;

 private:
  class Node final {
    const T key_;
    Node* left_;
    Node* right_;

   public:
    explicit Node(T key);

    void SetLeftChild(Node* node) noexcept;
    void SetRightChild(Node* node) noexcept;
    [[nodiscard]] Node*& GetLeftChild() noexcept;
    [[nodiscard]] Node*& GetRightChild() noexcept;
    void Destroy();

    [[nodiscard]] const T& GetKey() const noexcept;
    [[nodiscard]] bool Contains(T key) const;
  };

  Node* root_{nullptr};

  void SmallLeftRotation(Node*& root) const;
  void LargeLeftRotation(Node*& latest_root) const;
  void SmallRightRotation(Node*& root) const;
  void LargeRightRotation(Node*& latest_root) const;

  template<typename Comp>
  void BalancingSubtree(Node*& subtree, T destination_key, T source_key, Comp comp) const;

  Node* DeleteMinChild(Node* root);
  Node* FindMinChild(Node* root) const;
  bool InsertImpl(Node*& root, Node* added) const;
  Node* DeleteImpl(Node*& root, T deleted_key);
  void VisitImpl(Node* root, Visitor visitor) const;
  [[nodiscard]] int DepthImpl(Node* root) const;

 public:
  ~AVLTree();

  void Insert(T added_key);
  void Delete(T deleted_key);

  [[nodiscard]] bool Contains(T key) const;
  [[nodiscard]] int Depth() const;

  void Visit(Visitor visitor) const;
};
}  // namespace algo::tree

#define STRUCTURES_AVL_TREE_H_
#include <structures/tree/avl_tree-inl.hpp>