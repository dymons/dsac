#include <dsac/concurrency/fibers/details/stack/mmap_allocator.hpp>

#include <unistd.h>

#include <sys/mman.h>

namespace dsac {

mmap_allocator::mmap_allocator(char* start, size_t size) noexcept
  : start_(start)
  , size_(size) {
}

mmap_allocator::mmap_allocator(mmap_allocator&& that) {
  std::swap(start_, that.start_);
  std::swap(size_, that.size_);
}

mmap_allocator& mmap_allocator::operator=(mmap_allocator&& that) {
  std::swap(start_, that.start_);
  std::swap(size_, that.size_);
  return *this;
}

mmap_allocator::~mmap_allocator() {
  if (nullptr == start_) {
    return;
  }

  [[maybe_unused]] int ret = munmap(reinterpret_cast<void*>(start_), size_);
}

auto mmap_allocator::view() const noexcept -> std::span<char> {
  return {start_, size_};
}

auto mmap_allocator::get_page_bytes() -> std::size_t {
  static std::size_t kDefaultPageBytes = sysconf(_SC_PAGESIZE);
  return kDefaultPageBytes;
}

auto mmap_allocator::make(std::size_t pages) -> mmap_allocator {
  const auto bytes_to_allocate = pages * get_page_bytes();

  void* start = mmap(
      /*addr=*/nullptr,
      /*length=*/bytes_to_allocate,
      /*prot=*/PROT_READ | PROT_WRITE,
      /*flags=*/MAP_PRIVATE | MAP_ANONYMOUS,
      /*fd=*/-1,
      /*offset=*/0
  );

  return mmap_allocator{static_cast<char*>(start), bytes_to_allocate};
}

void mmap_allocator::protect_pages(std::size_t offset, std::size_t count) {
  [[maybe_unused]] int ret = mprotect(
      /*addr=*/reinterpret_cast<void*>(start_ + (offset * get_page_bytes())),
      /*len=*/count * get_page_bytes(),
      /*prot=*/PROT_NONE
  );
}

}  // namespace dsac
