#pragma once

#include "../../../../../../../../../../usr/include/c++/11/functional"

namespace dsac::tree {
template <typename T>
class RBTree final {
public:
  using Visitor = std::function<void(T)>;

private:
  enum class Color : unsigned char { Red, Black };
  struct Node final {
    T     key{};
    Node* left{nullptr};
    Node* right{nullptr};
    Node* parent{nullptr};
    Color color{Color::Red};

    [[nodiscard]] int                              MaxDepth() const;
    [[nodiscard]] int                              MinDepth() const;
    [[nodiscard]] bool                             Contains(T search_key) const;
    [[nodiscard, gnu::always_inline]] inline Node* GetParent() const noexcept;
    [[nodiscard, gnu::always_inline]] inline Node* GetGrandparent() const noexcept;
    [[nodiscard, gnu::always_inline]] inline Node* GetUncle() const noexcept;
    [[nodiscard, gnu::always_inline]] inline bool  IsColor(Color color) const noexcept;

    void Recolor() noexcept;
    void Destroy();
  };

  void SmallLeftRotation(Node* subtree);
  void SmallRightRotation(Node* subtree);
  void BalancingSubtree(Node* subtree);

  void VisitImpl(Node* root, Visitor visitor) const;

  Node* root_{nullptr};

public:
  RBTree()                         = default;
  RBTree(const RBTree&)            = delete;
  RBTree(RBTree&&)                 = delete;
  RBTree& operator=(const RBTree&) = delete;
  RBTree& operator=(RBTree&&)      = delete;
  ~RBTree();

  void Insert(T key);

  [[nodiscard]] int  MaxDepth() const;
  [[nodiscard]] int  MinDepth() const;
  [[nodiscard]] bool Contains(T key) const;
  void               Visit(Visitor visitor) const;
};
}  // namespace dsac::tree

#define STRUCTURES_RB_TREE_H_
#include "rb_tree-inl.hpp"
#undef STRUCTURES_RB_TREE_H_