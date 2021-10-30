#pragma once

#include <cassert>

namespace algo::tree {
template <typename T>
class AVLTree final {
  static constexpr char LeftHeavy = -1;
  static constexpr char Balanced = 0;
  static constexpr char RightHeavy = 1;

  class Node final {
    const T key_;
    char balance_factor_;
    Node* left_;
    Node* right_;

   public:
    explicit Node(T key)
        : key_(key), balance_factor_(Balanced), left_(nullptr), right_(nullptr) {
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

    void SetBalanceFactor(unsigned char balance_factor) {
      balance_factor_ = balance_factor;
    }

    [[nodiscard]] unsigned char GetBalanceFactor() const noexcept {
      return balance_factor_;
    }

    [[nodiscard]] const T& GetKey() const noexcept {
      return key_;
    }
  };

  Node* root_;

  void SmallLeftRotation(Node*& root) {
    Node* const heavy_right = root->GetRightChild();
    root->SetBalanceFactor(Balanced);
    heavy_right->SetBalanceFactor(Balanced);

    root->SetRightChild(heavy_right->GetLeftChild());
    heavy_right->SetLeftChild(root);

    root = heavy_right;
  }

  void LargeLeftRotation(Node*& root) {
    Node* const heavy_right = root->GetRightChild();
    Node* const heavy_left = heavy_left->GetLeftChild();

    if (heavy_left->GetBalanceFactor() == LeftHeavy) {
      root->SetBalanceFactor(Balanced);
      heavy_right->SetBalanceFactor(LeftHeavy);
    } else {
      root->SetBalanceFactor(LeftHeavy);
      heavy_right->SetBalanceFactor(Balanced);
    }
    heavy_left->SetBalanceFactor(Balanced);

    heavy_right->SetLeftChild(heavy_left->GetRightChild());
    heavy_left->SetRightChild(heavy_right);
    root->SetRightChild(heavy_left->GetLeftChild());
    heavy_left->SetLeftChild(root);

    root = heavy_left;
  }

  void SmallRightRotation(Node*& root) {
    Node* const heavy_left = root->GetLeftChild();
    root->SetBalanceFactor(Balanced);
    heavy_left->SetBalanceFactor(Balanced);

    root->SetLeftChild(heavy_left->GetRightChild());
    heavy_left->SetRightChild(root);

    root = heavy_left;
  }

  void LargeRightRotation(Node*& root) {
    Node* const heavy_left = root->GetLeftChild();
    Node* const heavy_right = heavy_left->GetRightChild();

    if (heavy_right->GetBalanceFactor() == RightHeavy) {
      root->SetBalanceFactor(Balanced);
      heavy_left->SetBalanceFactor(RightHeavy);
    } else {
      root->SetBalanceFactor(RightHeavy);
      heavy_left->SetBalanceFactor(Balanced);
    }
    heavy_right->SetBalanceFactor(Balanced);

    heavy_left->SetRightChild(heavy_right->GetLeftChild());
    heavy_right->SetLeftChild(heavy_left);
    root->SetLeftChild(heavy_right->GetRightChild());
    heavy_right->SetRightChild(root);

    root = heavy_right;
  }

  [[nodiscard]] bool BalancingLeftSubtreeImpl(Node*& root) {
    Node* left_child = root->GetLeftChild();
    const char left_child_balance_factor = left_child->GetBalanceFactor();
    if (left_child_balance_factor == LeftHeavy) {
      SmallRightRotation(root);
      return true;
    } else if (left_child_balance_factor == RightHeavy) {
      LargeRightRotation(root);
      return true;
    }

    return false;
  }

  [[nodiscard]] bool BalancingRightSubtreeImpl(Node*& root) {
    Node* right_child = root->GetRightChild();
    const char right_child_balance_factor = right_child->GetBalanceFactor();
    if (right_child_balance_factor == RightHeavy) {
      SmallLeftRotation(root);
      return true;
    } else if (right_child_balance_factor == LeftHeavy) {
      LargeLeftRotation(root);
      return true;
    }

    return false;
  }

  [[nodiscard]] bool BalancingLeftSubtree(Node*& root) {
    const char current_balance_factor = root->GetBalanceFactor();
    if (current_balance_factor == LeftHeavy) {
      return BalancingLeftSubtreeImpl(root);
    } else if (current_balance_factor == Balanced) {
      root->SetBalanceFactor(LeftHeavy);
      return true;
    }

    root->SetBalanceFactor(Balanced);
    return false;
  }

  [[nodiscard]] bool BalancingRightSubtree(Node*& root) {
    const char current_balance_factor = root->GetBalanceFactor();
    if (current_balance_factor == RightHeavy) {
      return BalancingRightSubtreeImpl(root);
    } else if (current_balance_factor == Balanced) {
      root->SetBalanceFactor(RightHeavy);
      return true;
    }

    root->SetBalanceFactor(Balanced);
    return false;
  }

  [[nodiscard]] bool InsertImpl(Node*& root, Node* added) {
    if (root == nullptr) {
      root = added;
      return true;
    } else if (const bool is_less = added->GetKey() < root->GetKey(); is_less) {
      if (const bool is_added = InsertImpl(root->GetLeftChild(), added); is_added) {
        return BalancingLeftSubtree(root);;
      }
    } else if (const bool is_more = added->GetKey() > root->GetKey(); is_more) {
      if (const bool is_added = InsertImpl(root->GetRightChild(), added); is_added) {
        return BalancingRightSubtree(root);;
      }
    }

    return false;
  }

 public:
  void Insert(T added_key) {
    Node* const new_node = new Node{added_key};
    if (const bool is_added = InsertImpl(root_, new_node); !is_added) {
      delete new_node;
    }
  }
};
}  // namespace algo::tree