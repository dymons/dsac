#pragma once

#include <functional>
#include <cassert>

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
    explicit Node(T key) : key_(key), left_(nullptr), right_(nullptr) {
    }

    void SetLeftChild(Node* node) noexcept {
      left_ = node;
    }

    void SetRightChild(Node* node) noexcept {
      right_ = node;
    }

    [[nodiscard]] Node*& GetLeftChild() noexcept {
      return left_;
    }

    [[nodiscard]] Node*& GetRightChild() noexcept {
      return right_;
    }

    [[nodiscard]] const T& GetKey() const noexcept {
      return key_;
    }

    [[nodiscard]] bool Contains(T key) const {
      if (key == key_) {
        return true;
      }

      return (left_ != nullptr && left_->Contains(key)) ||
             (right_ != nullptr && right_->Contains(key));
    }

    void Destroy() {
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
  };

  Node* root_{nullptr};

  void SmallLeftRotation(Node*& root) {
    Node* const heavy_right = root->GetRightChild();

    root->SetRightChild(heavy_right->GetLeftChild());
    heavy_right->SetLeftChild(root);

    root = heavy_right;
  }

  void LargeLeftRotation(Node*& latest_root) {
    Node* const right_child = latest_root->GetRightChild();
    Node* const new_root = right_child->GetLeftChild();

    right_child->SetLeftChild(new_root->GetRightChild());
    new_root->SetRightChild(right_child);
    latest_root->SetRightChild(new_root->GetLeftChild());
    new_root->SetLeftChild(latest_root);

    latest_root = new_root;
  }

  void SmallRightRotation(Node*& root) {
    Node* const heavy_left = root->GetLeftChild();

    root->SetLeftChild(heavy_left->GetRightChild());
    heavy_left->SetRightChild(root);

    root = heavy_left;
  }

  void LargeRightRotation(Node*& latest_root) {
    Node* const left_child = latest_root->GetLeftChild();
    Node* const new_root = left_child->GetRightChild();

    left_child->SetRightChild(new_root->GetLeftChild());
    new_root->SetLeftChild(left_child);
    latest_root->SetLeftChild(new_root->GetRightChild());
    new_root->SetRightChild(latest_root);

    latest_root = new_root;
  }

  bool InsertImpl(Node*& root, Node* added) {
    if (root == nullptr) {
      root = added;
      return true;
    } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
      if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
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

        return true;
      }
    } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
      if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
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

        return true;
      }
    }

    return false;
  }

  void VisitImpl(Node* root, Visitor visitor) const {
    if (root != nullptr) {
      VisitImpl(root->GetLeftChild(), visitor);
      visitor(root->GetKey());
      VisitImpl(root->GetRightChild(), visitor);
    }
  }

  [[nodiscard]] int DepthImpl(Node* root) const {
    return root == nullptr ? 0
                           : 1 + std::max(DepthImpl(root->GetLeftChild()),
                                          DepthImpl(root->GetRightChild()));
  }

 public:
  ~AVLTree() {
    if (root_ != nullptr) {
      root_->Destroy();
      delete root_;
      root_ = nullptr;
    }
  }

  void Insert(T added_key) {
    if (const bool exist = Contains(added_key); !exist) {
      InsertImpl(root_, new Node{added_key});
    }
  }

  void Visit(Visitor visitor) const {
    VisitImpl(root_, visitor);
  }

  [[nodiscard]] bool Contains(T key) const {
    return root_ != nullptr && root_->Contains(key);
  }

  [[nodiscard]] int Depth() const {
    return DepthImpl(root_) - 1;
  }
};
}  // namespace algo::tree