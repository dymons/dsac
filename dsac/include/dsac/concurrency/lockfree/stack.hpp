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

    template <std::same_as<T> U>
    static node* make(U&& u, node* next) {
      return new node{.value = std::forward<U>(u), .next = next};
    }
  };

public:
  DISALLOW_COPY_AND_MOVE(lock_free_stack);

  ~lock_free_stack() {
    erase(head_.load());
    erase(free_list_.load());
  }

  template <std::same_as<T> U>
  auto push(U&& u) -> void {
    auto* new_node = node::make(std::forward<U>(u), head_.load());
    while (!head_.compare_exchange_weak(new_node->next, new_node)) {
    }
  }

  auto pop() -> std::optional<T> {
    ++popped_count;
    for (auto* current = head_.load(); nullptr != current;) {
      if (head_.compare_exchange_weak(current, current->next)) {
        auto value = std::move(current->value);

        try_to_erase_free_list();
        if (--popped_count == 0) {
          delete current;
        } else {
          current->next = free_list_.load();
          while (!free_list_.compare_exchange_weak(current->next, current)) {
          }
        }

        return std::move(value);
      }
    }

    try_to_erase_free_list();
    --popped_count;
    return std::nullopt;
  }

private:
  auto erase(auto* current) {
    while (nullptr != current) {
      auto* next = current->next;
      delete current;
      current = next;
    }
  }

  auto try_to_erase_free_list() {
    if (auto* current = free_list_.load(); nullptr != current) {
      if (popped_count == 1) {
        if (free_list_.compare_exchange_strong(current, nullptr)) {
          erase(current);
        }
      }
    }
  }

  std::atomic<node*>       head_{nullptr};
  std::atomic<node*>       free_list_{nullptr};
  std::atomic<std::size_t> popped_count{0};
};

}  // namespace dsac
