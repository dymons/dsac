#include <dsac/concurrency/fibers/details/stack/stack.hpp>

namespace dsac {

namespace {

inline constexpr std::size_t operator""_KiB(unsigned long long count) {
  return count * 1024;
}

const auto kDefaultStackBytes = stack_bytes{64_KiB};

}  // namespace

fiber_stack::fiber_stack(mmap_allocator fiber_stack_allocator) noexcept
  : fiber_stack_allocator_(std::move(fiber_stack_allocator)) {
}

auto fiber_stack::make(stack_bytes need_bytes) -> fiber_stack {
  static const auto kDefaultPageBytes = mmap_allocator::get_page_bytes();

  auto pages = need_bytes.get() / kDefaultPageBytes;
  if ((need_bytes.get() % kDefaultPageBytes) != 0) {
    ++pages;
  }

  return fiber_stack::make(stack_pages{pages});
}

auto fiber_stack::make(mmap_allocator fiber_stack_allocator) -> fiber_stack {
  return fiber_stack{std::move(fiber_stack_allocator)};
}

auto fiber_stack::make(stack_pages need_pages) -> fiber_stack {
  auto stack = mmap_allocator::make(need_pages.get());
  stack.protect_pages(/*offset=*/0UL, /*count=*/1UL);
  return fiber_stack::make(std::move(stack));
}

auto fiber_stack::view() const noexcept -> std::span<char> {
  return fiber_stack_allocator_.view();
}

auto fiber_stack_allocator::allocate() -> fiber_stack {
  if (stacks_.empty()) {
    return fiber_stack::make(kDefaultStackBytes);
  }

  fiber_stack stack = std::move(stacks_.back());
  stacks_.pop_back();
  return stack;
}

auto fiber_stack_allocator::release(fiber_stack stack) -> void {
  stacks_.push_back(std::move(stack));
}

}  // namespace dsac