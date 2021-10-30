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

    void SetLeftChild(Node* node) {
      left_ = node;
    }

    void SetRightChild(Node* node) {
      right_ = node;
    }

    [[nodiscard]] Node*& GetLeftChild() noexcept {
      return left_;
    }

    [[nodiscard]] Node*& GetRightChild() noexcept {
      return right_;
    }

    void SetBalanceFactor(BalanceStatus balance_factor) {
      balance_factor_ = balance_factor;
    }

    [[nodiscard]] BalanceStatus GetBalanceFactor() const noexcept {
      return balance_factor_;
    }

    [[nodiscard]] const T& GetKey() const noexcept {
      return key_;
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

  [[nodiscard]] bool BalancingLeftSubtreeImpl(Node*& root) {
    Node* left_child = root->GetLeftChild();
    const BalanceStatus left_child_balance_factor = left_child->GetBalanceFactor();
    if (left_child_balance_factor == BalanceStatus::LeftHeavy) {
      SmallRightRotation(root);
      return true;
    } else if (left_child_balance_factor == BalanceStatus::RightHeavy) {
      LargeRightRotation(root);
      return true;
    }

    return false;
  }

  [[nodiscard]] bool BalancingRightSubtreeImpl(Node*& root) {
    Node* right_child = root->GetRightChild();
    const BalanceStatus right_child_balance_factor = right_child->GetBalanceFactor();
    if (right_child_balance_factor == BalanceStatus::RightHeavy) {
      SmallLeftRotation(root);
      return true;
    } else if (right_child_balance_factor == BalanceStatus::LeftHeavy) {
      LargeLeftRotation(root);
      return true;
    }

    return false;
  }

  [[nodiscard]] bool BalancingLeftSubtree(Node*& root) {
    const BalanceStatus current_balance_factor = root->GetBalanceFactor();
    if (current_balance_factor == BalanceStatus::LeftHeavy) {
      return BalancingLeftSubtreeImpl(root);
    } else if (current_balance_factor == BalanceStatus::Balanced) {
      root->SetBalanceFactor(BalanceStatus::LeftHeavy);
      return true;
    }

    root->SetBalanceFactor(BalanceStatus::Balanced);
    return false;
  }

  [[nodiscard]] bool BalancingRightSubtree(Node*& root) {
    const BalanceStatus current_balance_factor = root->GetBalanceFactor();
    if (current_balance_factor == BalanceStatus::RightHeavy) {
      return BalancingRightSubtreeImpl(root);
    } else if (current_balance_factor == BalanceStatus::Balanced) {
      root->SetBalanceFactor(BalanceStatus::RightHeavy);
      return true;
    }

    root->SetBalanceFactor(BalanceStatus::Balanced);
    return false;
  }

  [[nodiscard]] bool InsertImpl(Node*& root, Node* added) {
    if (root == nullptr) {
      root = added;
      return true;
    } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
      if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
        return BalancingLeftSubtree(root);
      }
    } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
      if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
        return BalancingRightSubtree(root);
      }
    }

    return false;
  }

  void VisitImpl(Node* root, Visitor visitor) {
    if (root != nullptr) {
      VisitImpl(root->GetLeftChild(), visitor);
      visitor(root->GetKey());
      VisitImpl(root->GetRightChild(), visitor);
    }
  }

 public:
  void Insert(T added_key) {
    Node* const new_node = new Node{added_key};
    if (const bool is_added = InsertImpl(root_, new_node); !is_added) {
      delete new_node;
    }
  }

  void Visit(Visitor visitor) {
    VisitImpl(root_, visitor);
  }
};
}  // namespace algo::tree