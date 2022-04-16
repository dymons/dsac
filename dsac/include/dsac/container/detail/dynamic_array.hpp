#pragma once

#include <dsac/container/detail/construct.hpp>
#include <dsac/container/detail/normal_iterator.hpp>
#include <memory>
#include <stdexcept>

namespace dsac::detail {

template <typename T, typename Allocator>
struct DynamicArrayBase {
  using rebind_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
  using pointer               = typename std::allocator_traits<Allocator>::pointer;

  struct DynamicArrayData {
    pointer start;
    pointer finish;
    pointer end_of_storage;

    DynamicArrayData() noexcept
      : start(nullptr)
      , finish(nullptr)
      , end_of_storage(nullptr) {
    }

    DynamicArrayData(DynamicArrayData const&) = delete;
    DynamicArrayData(DynamicArrayData&& other) noexcept
      : start(other.start)
      , finish(other.finish)
      , end_of_storage(other.end_of_storage) {
      other.start = other.finish = other.end_of_storage = pointer{};
    }

    DynamicArrayData& operator=(DynamicArrayData const&)     = delete;
    DynamicArrayData& operator=(DynamicArrayData&&) noexcept = delete;

    ~DynamicArrayData() = default;

    void copy_data(DynamicArrayData const& other) noexcept {
      start          = other.start;
      finish         = other.finish;
      end_of_storage = other.end_of_storage;
    }

    void swap_data(DynamicArrayData& other) noexcept {
      DynamicArrayData tmp;
      tmp.copy_data(*this);
      copy_data(other);
      other.copy_data(tmp);
    }
  };

  using allocator_type = Allocator;

  DynamicArrayBase() noexcept(std::is_nothrow_default_constructible<rebind_allocator_type>::value) = default;

  explicit DynamicArrayBase(allocator_type const& allocator) noexcept
    : allocator_(allocator) {
  }
  explicit DynamicArrayBase(allocator_type&& allocator) noexcept
    : allocator_(std::move(allocator)) {
  }
  DynamicArrayBase(allocator_type&& allocator, DynamicArrayBase&& other) noexcept
    : allocator_(std::move(allocator))
    , storage_(std::move(other)) {
  }
  DynamicArrayBase(size_t n, const allocator_type& allocator)
    : allocator_(allocator) {
    storage_.start          = dsac::allocate(n, this->allocator_);
    storage_.finish         = storage_.start;
    storage_.end_of_storage = storage_.start + n;
  }

  DynamicArrayBase(DynamicArrayBase const&) = delete;
  DynamicArrayBase(DynamicArrayBase&& other) noexcept
    : allocator_(std::move(other.allocator_))
    , storage_(std::move(other.storage_)) {
  }

  ~DynamicArrayBase() noexcept {
    dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
  }

  rebind_allocator_type allocator_;
  DynamicArrayData      storage_;
};

template <typename T, typename Allocator>
class DynamicArray : protected DynamicArrayBase<T, Allocator> {
  using base                    = DynamicArrayBase<T, Allocator>;
  using rebind_allocator_type   = typename base::rebind_allocator_type;
  using rebind_allocator_traits = typename std::allocator_traits<rebind_allocator_type>;

public:
  using value_type             = T;
  using allocator_type         = typename base::allocator_type;
  using allocator_traits       = typename std::allocator_traits<allocator_type>;
  using size_type              = typename allocator_traits::size_type;
  using difference_type        = typename allocator_traits::difference_type;
  using reference              = value_type&;
  using const_reference        = value_type const&;
  using pointer                = typename allocator_traits::pointer;
  using const_pointer          = typename allocator_traits::const_pointer;
  using iterator               = NormalIterator<pointer, DynamicArray>;
  using const_iterator         = NormalIterator<const_pointer, DynamicArray>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

protected:
  using base::allocator_;
  using base::storage_;

public:
  explicit DynamicArray(allocator_type allocator = allocator_type{}) noexcept
    : base(std::move(allocator)) {
  }

  explicit DynamicArray(
      size_type n, const value_type& value = value_type{}, const allocator_type& allocator = allocator_type())
    : base(n, allocator) {
    dsac::uninitialized_fill_n(storage_.start, n, value, allocator);
    storage_.finish = storage_.end_of_storage;
  }

  DynamicArray(std::initializer_list<T> list, allocator_type allocator = allocator_type{})
    : base(std::move(allocator)) {
    const size_type size    = list.size();
    storage_.start          = dsac::allocate(size, allocator);
    storage_.end_of_storage = storage_.start + size;
    storage_.finish         = dsac::uninitialized_copy(list.begin(), list.end(), storage_.start, allocator);
  }

  DynamicArray(DynamicArray const& other)
    : base(other.size(), rebind_allocator_traits::select_on_container_copy_construction(other.allocator_)) {
    storage_.finish = dsac::uninitialized_copy(other.begin(), other.end(), storage_.start, allocator_);
  }

  DynamicArray(DynamicArray&& other) noexcept = default;

  DynamicArray& operator=(DynamicArray const& x) {
    if (&x != this) {
      if (allocator_traits::propagate_on_container_copy_assignment::value) {
        bool const is_always_equal     = allocator_traits::is_always_equal::value;
        bool const is_allocators_equal = allocator_ == x.allocator_;
        if (is_always_equal && !is_allocators_equal) {
          clear();
          dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
          storage_.start          = nullptr;
          storage_.finish         = nullptr;
          storage_.end_of_storage = nullptr;
        }
        dsac::alloc_on_copy(allocator_, x.allocator_);
      }

      const size_type xsize = x.size();
      if (xsize > capacity()) {
        pointer tmp = dsac::allocate_and_copy(xsize, x.begin(), x.end(), allocator_);
        dsac::destroy(storage_.start, storage_.finish, allocator_);
        dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
        storage_.start          = tmp;
        storage_.end_of_storage = storage_.start + xsize;
      } else if (size() >= xsize) {
        dsac::destroy(std::copy(x.begin(), x.end(), begin()), end(), allocator_);
      } else {
        std::copy(x.storage_.start, x.storage_.start + size(), storage_.start);
        dsac::uninitialized_copy(x.storage_.start + size(), x.storage_.finish, storage_.finish, allocator_);
      }
      storage_.finish = storage_.start + xsize;
    }
    return *this;
  }

  DynamicArray& operator=(DynamicArray&& other) = delete; // Sorry, unimplemented yet ¯\_(ツ)_/¯

  ~DynamicArray() noexcept {
    dsac::destroy(storage_.start, storage_.finish, allocator_);
  }

  [[nodiscard]] inline size_type size() const noexcept {
    return size_type(storage_.finish - storage_.start);
  }

  [[nodiscard]] inline size_type capacity() const noexcept {
    return size_type(storage_.end_of_storage - storage_.start);
  }

  void push_back(value_type const& value) {
    if (storage_.finish != storage_.end_of_storage) {
      dsac::construct(storage_.finish, allocator_, value);
      ++storage_.finish;
    } else {
      realloc_and_insert(value);
    }
  }

  [[nodiscard]] iterator begin() noexcept {
    return iterator(storage_.start);
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator(storage_.start);
  }

  [[nodiscard]] iterator end() noexcept {
    return iterator(storage_.finish);
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator(storage_.finish);
  }

  [[nodiscard]] reference operator[](size_type n) noexcept {
    return *(storage_.start + n);
  }

  [[nodiscard]] bool empty() const noexcept {
    return begin() == end();
  }

  void clear() noexcept {
    dsac::destroy(storage_.start, storage_.finish, allocator_);
    storage_.finish = storage_.start;
  }

private:
  size_type twice_size(size_type current_size) const {
    const size_type max_size = allocator_traits::max_size(allocator_);
    if (max_size - current_size < 1U) {
      throw std::range_error{"Not enough memory to allocate"};
    }

    const size_type len = current_size + std::max<size_type>(current_size, 1U);
    return (len < current_size || len > max_size) ? max_size : len;
  }

  template <typename... Args>
  void realloc_and_insert(Args&&... args) {
    const size_type new_size   = twice_size(size());
    pointer         new_start  = dsac::allocate(new_size, allocator_);
    pointer         new_finish = pointer{};

    try {
      dsac::construct(new_start + size(), allocator_, std::forward<Args>(args)...);
      new_finish = dsac::uninitialized_move_if_noexcept(storage_.start, storage_.finish, new_start, allocator_);
    } catch (...) {
      if (!new_finish) {
        allocator_traits::destroy(allocator_, new_start + size());
      } else {
        dsac::destroy(new_start, new_finish, allocator_);
      }

      dsac::deallocate(new_start, new_size, allocator_);
      throw;
    }

    dsac::destroy(storage_.start, storage_.finish, allocator_);
    dsac::deallocate(storage_.start, capacity(), allocator_);

    storage_.start          = new_start;
    storage_.finish         = ++new_finish;
    storage_.end_of_storage = new_start + new_size;
  }
};

template <class T, class Allocator>
bool operator==(const DynamicArray<T, Allocator>& lhs, const DynamicArray<T, Allocator>& rhs) {
  using std::begin;
  using std::end;
  return std::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

}  // namespace dsac::detail
