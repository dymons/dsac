#pragma once

#include <span>

namespace dsac {

class mmap_allocator final {
  char*  start_{};
  size_t size_{};

  // Constructors

  mmap_allocator(char* start, size_t size) noexcept;

public:
  // Constructors

  mmap_allocator() = default;

  mmap_allocator(const mmap_allocator&) = delete;

  mmap_allocator(mmap_allocator&& that);

  [[nodiscard]] static auto make(std::size_t pages) -> mmap_allocator;

  // Destructor

  ~mmap_allocator();

  // Assignment

  mmap_allocator& operator=(const mmap_allocator&) = delete;

  mmap_allocator& operator=(mmap_allocator&& that);

  // Observers

  [[nodiscard]] auto view() const noexcept -> std::span<char>;

  [[nodiscard]] static auto get_page_bytes() -> std::size_t;
};

}  // namespace dsac