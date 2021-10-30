#pragma once

#include <functional>
#include <cassert>

namespace algo::tree {
template <typename T>
class AVLTree final {
 public:
  using Visitor = std::function<void(T)>;

 private:
  enum class BalanceStatus : char { LeftHeavy, Balanced, RightHeavy };

  class Node final {
    const T key_;
    BalanceStatus balance_factor_;
    Node* left_;
    Node* right_;

   public:
    explicit Node(T key)
        : key_(key),
          balance_factor_(BalanceStatus::Balanced),
          left_(nullptr),
          right_(nullptr) {
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

    void SetBalanceFactor(BalanceStatus balance_factor) noexcept {
      balance_factor_ = balance_factor;
    }

    [[nodiscard]] BalanceStatus GetBalanceFactor() const noexcept {
      return balance_factor_;
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
    root->SetBalanceFactor(BalanceStatus::Balanced);
    heavy_right->SetBalanceFactor(BalanceStatus::Balanced);

    root->SetRightChild(heavy_right->GetLeftChild());
    heavy_right->SetLeftChild(root);

    root = heavy_right;
  }

  void LargeLeftRotation(Node*& latest_root) {
    Node* const right_child = latest_root->GetRightChild();
    Node* const new_root = right_child->GetLeftChild();

    if (new_root->GetBalanceFactor() == BalanceStatus::LeftHeavy) {
      latest_root->SetBalanceFactor(BalanceStatus::Balanced);
      right_child->SetBalanceFactor(BalanceStatus::LeftHeavy);
    } else {
      latest_root->SetBalanceFactor(BalanceStatus::LeftHeavy);
      right_child->SetBalanceFactor(BalanceStatus::Balanced);
    }
    new_root->SetBalanceFactor(BalanceStatus::Balanced);

    right_child->SetLeftChild(new_root->GetRightChild());
    new_root->SetRightChild(right_child);
    latest_root->SetRightChild(new_root->GetLeftChild());
    new_root->SetLeftChild(latest_root);

    latest_root = new_root;
  }

  void SmallRightRotation(Node*& root) {
    Node* const heavy_left = root->GetLeftChild();
    root->SetBalanceFactor(BalanceStatus::Balanced);
    heavy_left->SetBalanceFactor(BalanceStatus::Balanced);

    root->SetLeftChild(heavy_left->GetRightChild());
    heavy_left->SetRightChild(root);

    root = heavy_left;
  }

  void LargeRightRotation(Node*& latest_root) {
    Node* const left_child = latest_root->GetLeftChild();
    Node* const new_root = left_child->GetRightChild();

    if (new_root->GetBalanceFactor() == BalanceStatus::RightHeavy) {
      latest_root->SetBalanceFactor(BalanceStatus::Balanced);
      left_child->SetBalanceFactor(BalanceStatus::RightHeavy);
    } else {
      latest_root->SetBalanceFactor(BalanceStatus::RightHeavy);
      left_child->SetBalanceFactor(BalanceStatus::Balanced);
    }
    new_root->SetBalanceFactor(BalanceStatus::Balanced);

    left_child->SetRightChild(new_root->GetLeftChild());
    new_root->SetLeftChild(left_child);
    latest_root->SetLeftChild(new_root->GetRightChild());
    new_root->SetRightChild(latest_root);

    latest_root = new_root;
  }

  void BalancingLeftSubtreeImpl(Node*& root) {
    Node* left_child = root->GetLeftChild();
    const BalanceStatus status = left_child->GetBalanceFactor();
    if (status == BalanceStatus::LeftHeavy) {
      SmallRightRotation(root);
    } else if (status == BalanceStatus::RightHeavy) {
      LargeRightRotation(root);
    }
  }

  void BalancingRightSubtreeImpl(Node*& root) {
    Node* right_child = root->GetRightChild();
    const BalanceStatus status = right_child->GetBalanceFactor();
    if (status == BalanceStatus::RightHeavy) {
      SmallLeftRotation(root);
    } else if (status == BalanceStatus::LeftHeavy) {
      LargeLeftRotation(root);
    }
  }

  void BalancingLeftSubtree(Node*& root) {
    const BalanceStatus status = root->GetBalanceFactor();
    if (status == BalanceStatus::LeftHeavy) {
      return BalancingLeftSubtreeImpl(root);
    } else if (status == BalanceStatus::Balanced) {
      root->SetBalanceFactor(BalanceStatus::LeftHeavy);
    } else {
      root->SetBalanceFactor(BalanceStatus::Balanced);
    }
  }

  void BalancingRightSubtree(Node*& root) {
    const BalanceStatus status = root->GetBalanceFactor();
    if (status == BalanceStatus::RightHeavy) {
      return BalancingRightSubtreeImpl(root);
    } else if (status == BalanceStatus::Balanced) {
      root->SetBalanceFactor(BalanceStatus::RightHeavy);
    } else {
      root->SetBalanceFactor(BalanceStatus::Balanced);
    }
  }

  bool InsertImpl(Node*& root, Node* added) {
    if (root == nullptr) {
      root = added;
      return true;
    } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
      if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
        BalancingLeftSubtree(root);
        return true;
      }
    } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
      if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
        BalancingRightSubtree(root);
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
};
}  // namespace algo::tree