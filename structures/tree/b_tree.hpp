#pragma once

#include <vector>
#include <cassert>

namespace algo::tree {
template <typename T>
class BTree final {
  class Node final {
    /// Минимальная степень
    int t_ = 0;
    std::vector<T> keys_;
    std::vector<Node*> nodes_;

   public:
    explicit Node(int t) : t_(t) {
    }

    void AddKey(T key) {
      const auto upper = std::upper_bound(keys_.begin(), keys_.end(), key);
      keys_.insert(upper, key);
    }

    void AddKeyByIndex(std::size_t index, T key) {
      keys_.insert(std::next(keys_.begin(), index), key);
    }

    [[nodiscard]] T GetKey(std::size_t index) const {
      assert(index < keys_.size());
      return keys_[index];
    }

    void RemoveKey(T key) {
      auto it = std::find(keys_.begin(), keys_.end(), key);
      keys_.erase(it);
    }

    void AddNode(Node* node) {
      nodes_.push_back(node);
    }

    void AddNodeByIndex(std::size_t index, Node* node) {
      nodes_.insert(std::next(nodes_.begin(), index), node);
    }

    [[nodiscard]] Node* GetNode(std::size_t index) const {
      assert(index < nodes_.size());
      return nodes_[index];
    }

    void RemoveNode(Node* node) {
      auto it = std::find(nodes_.begin(), nodes_.end(), node);
      nodes_.erase(it);
    }

    [[nodiscard]] bool IsLeaf() const noexcept {
      return nodes_.empty();
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
        if (Node* const consider_node = GetNode(index); consider_node->IsKeysFull()) {
          SplitChild(index, consider_node);
          if (GetKey(index) < key) {
            index++;
          }
        }

        GetNode(index)->InsertNonFull(key);
      }
    }

    void SplitChild(std::size_t index, Node* root) {
      Node* const new_node = new Node{.t = root->t_};
      for (int j = 0; j < t_ - 1; j++) {
        new_node->AddKey(root->GetKey(j + t_));
      }

      if (!root->IsLeaf()) {
        for (int j = 0; j < t_ - 1; j++) {
          new_node->AddNode(root->GetNode(j + t_));
        }
      }

      AddNodeByIndex(index + 1, new_node);
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
        new_node->AddNode(root_);
        new_node->SplitChild(0, root_);

        const std::size_t index = new_node->GetKey(0) < key ? 1 : 0;
        new_node->GetNode(index)->InsertNonFull(key);

        root_ = new_node;
      } else {
        root_->InsertNonFull(key);
      }
    }
  }
};
}  // namespace algo::tree