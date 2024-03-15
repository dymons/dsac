#pragma once

#include <optional>
#include <stack>

#include <dsac/concurrency/synchronization/mutex.hpp>
#include <dsac/concurrency/synchronization/unique_lock.hpp>

namespace dsac {

template <typename T>
class blocking_stack final {
public:
  auto push(T value) -> void {
    [[maybe_unused]] auto guard = unique_lock{mutex_};
    stack_.push(std::move(value));
  }

  auto pop() -> std::optional<T> {
    [[maybe_unused]] auto guard = unique_lock{mutex_};
    if (!stack_.empty()) {
      auto value = std::move(stack_.top());
      stack_.pop();
      return std::move(value);
    }

    return std::nullopt;
  }

private:
  mutex         mutex_;
  std::stack<T> stack_;
};

}  // namespace dsac