#pragma once

#include <atomic>
#include <optional>

namespace dsac {

template <typename T>
class lock_free_stack final {
  struct Node {
    T     value{};
    Node* next{nullptr};
  };

public:
  auto push(T value) -> void {
    auto new_node = new Node{
        .value = std::move(value),
        .next  = head_.load(),
    };

    while (!head_.compare_exchange_weak(new_node->next, new_node)) {
    };
  }

  auto pop() -> std::optional<T> {
    while (true) {
      auto* head = head_.load();
      if (head == nullptr) {
        return std::nullopt;
      } else if (head_.compare_exchange_weak(head, head->next)) {
        return std::move(head->value);
      }
    }
  }

private:
  std::atomic<Node*> head_{nullptr};
};

}  // namespace dsac
