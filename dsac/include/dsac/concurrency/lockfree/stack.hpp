#pragma once

#include <atomic>
#include <optional>

#include <dsac/common/macros.h>

namespace dsac {

template <typename T>
class lock_free_stack final {
  struct node final {
    T     value{};
    node* next{nullptr};

    static node* make(T&& v, node* next) {
      return new node{.value = std::move(v), .next = next};
    }
  };

public:
  DISALLOW_COPY_AND_MOVE(lock_free_stack);

  ~lock_free_stack() {
    auto* current = head_.load();
    while (nullptr != current) {
      auto* next = current->next;
      delete current;
      current = next;
    }
  }

  template <std::same_as<T> U>
  auto push(U&& u) -> void {
    auto* new_node = node::make(std::forward<U>(u), head_.load());
    while (!head_.compare_exchange_weak(new_node->next, new_node)) {
    }
  }

  auto pop() -> std::optional<T> {
    for (auto* current = head_.load(); nullptr != current;) {
      if (head_.compare_exchange_weak(current, current->next)) {
        return std::move(current->value);
      }
    }
    return std::nullopt;
  }

private:
  std::atomic<node*> head_{nullptr};
};

}  // namespace dsac
