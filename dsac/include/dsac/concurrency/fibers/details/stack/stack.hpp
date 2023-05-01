#pragma once

#include "dsac/functional/strong_type.hpp"
#include "mmap_allocator.hpp"

#include <vector>

namespace dsac {

using stack_bytes = strong_type<std::size_t, struct StackBytes>;
using stack_pages = strong_type<std::size_t, struct StackPages>;

class fiber_stack final {
  mmap_allocator fiber_stack_allocator_{};

  // Constructors

  explicit fiber_stack(mmap_allocator fiber_stack_allocator) noexcept;

public:
  // Constructors

  fiber_stack() = default;

  [[nodiscard]] static auto make(stack_bytes need_bytes) -> fiber_stack;
  [[nodiscard]] static auto make(stack_pages need_pages) -> fiber_stack;
  [[nodiscard]] static auto make(mmap_allocator fiber_stack_allocator) -> fiber_stack;

  // Observers

  [[nodiscard]] auto view() const noexcept -> std::span<char>;
};

class fiber_stack_allocator final {
  std::vector<fiber_stack> stacks_;

public:
  // Constructors

  [[nodiscard]] auto allocate() -> fiber_stack;

  // Modifiers

  auto release(fiber_stack stack) -> void;
};

}  // namespace dsac