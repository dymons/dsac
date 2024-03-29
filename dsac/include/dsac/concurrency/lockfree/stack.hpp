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
    erase(stack_.load());
    erase(free_stack_.load());
  }

  template <std::same_as<T> U>
  auto push(U&& u) -> void {
    auto* new_node = node::make(std::forward<U>(u), stack_.load());
    push_to(stack_, new_node);
  }

  auto pop() -> std::optional<T> {
    ++popped_count;
    for (auto* current = stack_.load(); nullptr != current;) {
      if (stack_.compare_exchange_weak(current, current->next)) {
        auto value = std::move(current->value);

        try_to_erase_free_list();

        if (--popped_count == 0) {
          delete current;
        } else {
          // deferred reclamation
          current->next = free_stack_.load();
          push_to(free_stack_, current);
        }

        return std::move(value);
      }
    }

    try_to_erase_free_list();
    --popped_count;

    return std::nullopt;
  }

private:
  [[clang::always_inline]] auto push_to(auto& head, auto* new_head) -> void {
    while (!head.compare_exchange_weak(new_head->next, new_head)) {
    }
  }

  auto erase(auto* current) -> void {
    while (nullptr != current) {
      auto* next = current->next;
      delete current;
      current = next;
    }
  }

  auto try_to_erase_free_list() -> void {
    if (auto* current = free_stack_.load(); nullptr != current) {
      if (popped_count == 1) {
        if (free_stack_.compare_exchange_strong(current, nullptr)) {
          erase(current);
        }
      }
    }
  }

  std::atomic<node*>       stack_{nullptr};
  std::atomic<node*>       free_stack_{nullptr};
  std::atomic<std::size_t> popped_count{0};
};

}  // namespace dsac
