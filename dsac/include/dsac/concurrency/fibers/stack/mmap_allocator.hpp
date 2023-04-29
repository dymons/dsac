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

  [[nodiscard]] static auto make(std::size_t pages) -> mmap_allocator;

  // Observers

  [[nodiscard]] auto view() const noexcept -> std::span<char>;

  [[nodiscard]] static auto get_page_bytes() -> std::size_t;
};

}  // namespace dsac