#pragma once

#include <vector>

namespace algo::tree {
template <typename T>
class BTree final {
  class Node final {
    const int t_;
    std::vector<T> keys_;
    std::vector<Node*> children_;

    void AddKeyImpl(T key);

   public:
    explicit Node(int t);

    void AddKey(T key);
    void AddKeyByIndex(std::size_t index, T key);
    void RemoveKey(T key);
    void AddChild(Node* node);
    void AddChildByIndex(std::size_t index, Node* node);
    void SplitChild(std::size_t index, Node* root);
    void Destroy();

    [[nodiscard]] T const& GetKey(std::size_t index) const;
    [[nodiscard]] Node* GetChild(std::size_t index) const;
    [[nodiscard]] bool IsLeaf() const noexcept;
    [[nodiscard]] bool IsKeysFull() const noexcept;
    [[nodiscard]] bool Contains(T key) const;
  };

  const int t_;
  Node* root_;

 public:
  explicit BTree(int t);
  ~BTree();

  void Insert(T key);

  [[nodiscard]] bool Contains(T key) const;
};
}  // namespace algo::tree

#define STRUCTURES_B_TREE_H_
#include <structures/tree/b_tree-inl.hpp>
#undef STRUCTURES_B_TREE_H_