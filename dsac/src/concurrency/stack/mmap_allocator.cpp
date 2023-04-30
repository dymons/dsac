#include <dsac/concurrency/fibers/stack/mmap_allocator.hpp>

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
  if (start_ == nullptr) {
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
  const auto length = pages * get_page_bytes();

  void* start = mmap(
      /*addr=*/nullptr,
      /*length=*/length,
      /*prot=*/PROT_READ | PROT_WRITE,
      /*flags=*/MAP_PRIVATE | MAP_ANONYMOUS,
      /*fd=*/-1,
      /*offset=*/0
  );

  return mmap_allocator{static_cast<char*>(start), length};
}

}  // namespace dsac
