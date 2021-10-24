#pragma once

#include <vector>
#include <cassert>

namespace algo::tree {
template <typename T>
class BTree final {
  class Node final {
    /// Минимальная степень B-дерева
    int t_;

    /// Ключи узла
    std::vector<T> keys_;

    /// Потомки узла
    std::vector<Node*> children_;

   public:
    explicit Node(int t) : t_(t) {
    }

    void AddKey(T key) {
      keys_.insert(std::upper_bound(keys_.begin(), keys_.end(), key), key);
      // Ключи хранятся в отсортированном виде, проверяем инвариант
      // состояния узла на выполнение данного условия.
      assert(std::is_sorted(keys_.begin(), keys_.end()));
    }

    void AddKeyByIndex(std::size_t index, T key) {
      keys_.insert(std::next(keys_.begin(), index), key);
      // Ключи хранятся в отсортированном виде, проверяем инвариант
      // состояния узла на выполнение данного условия.
      assert(std::is_sorted(keys_.begin(), keys_.end()));
    }

    [[nodiscard]] T const& GetKey(std::size_t index) const {
      assert(index < keys_.size());
      return keys_[index];
    }

    void RemoveKey(const T& key) {
      keys_.erase(std::find(keys_.begin(), keys_.end(), key));
    }

    void AddChild(Node* node) {
      children_.push_back(node);
    }

    void AddChildByIndex(std::size_t index, Node* node) {
      children_.insert(std::next(children_.begin(), index), node);
    }

    [[nodiscard]] Node* GetChild(std::size_t index) const {
      assert(index < children_.size());
      return children_[index];
    }

    void RemoveNode(Node* node) {
      auto it = std::find(children_.begin(), children_.end(), node);
      children_.erase(it);
    }

    [[nodiscard]] bool IsLeaf() const noexcept {
      return children_.empty();
    }

    [[nodiscard]] bool IsKeysFull() const noexcept {
      return keys_.size() == (2 * t_ - 1);
    }

    void InsertNonFull(T key) {
      if (IsLeaf()) {
        AddKey(key);
      } else {
        const auto upper = std::upper_bound(keys_.begin(), keys_.end(), key);
        int index = std::distance(keys_.begin(), upper);
        if (Node* const consider_node = GetChild(index); consider_node->IsKeysFull()) {
          SplitChild(index, consider_node);
          if (GetKey(index) < key) {
            index++;
          }
        }

        GetChild(index)->InsertNonFull(key);
      }
    }

    void SplitChild(std::size_t index, Node* root) {
      Node* const new_node = new Node{.t = root->t_};
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
  };

  int t;
  Node* root_;

 public:
  explicit BTree(int t) : t(t), root_(nullptr) {
  }

  void Add(T key) {
    if (root_ == nullptr) {
      root_ = new Node{.t = t};
      root_->AddKey(key);
    } else {
      if (root_->IsKeysFull()) {
        Node* const new_node = new Node{.t = t};
        new_node->AddChild(root_);
        new_node->SplitChild(0, root_);

        const std::size_t index = new_node->GetKey(0) < key ? 1 : 0;
        new_node->GetChild(index)->InsertNonFull(key);

        root_ = new_node;
      } else {
        root_->InsertNonFull(key);
      }
    }
  }
};
}  // namespace algo::tree