#pragma once

#include <dsac/container/detail/construct.hpp>
#include <dsac/container/detail/normal_iterator.hpp>
#include <memory>
#include <stdexcept>

namespace dsac::detail {

template <typename T, typename Allocator>
struct DynamicArrayBase {
  // clang-format off
  using rebind_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
  using pointer               = typename std::allocator_traits<Allocator>::pointer;
  // clang-format on

  struct DynamicArrayData {
    pointer start;
    pointer finish;
    pointer end_of_storage;

    DynamicArrayData() noexcept
      : start(nullptr)
      , finish(nullptr)
      , end_of_storage(nullptr)
    {
    }

    DynamicArrayData(DynamicArrayData const&) = delete;
    DynamicArrayData(DynamicArrayData&& other) noexcept
      : start(other.start)
      , finish(other.finish)
      , end_of_storage(other.end_of_storage)
    {
      other.start = other.finish = other.end_of_storage = pointer{};
    }

    DynamicArrayData& operator=(DynamicArrayData const&) = delete;
    DynamicArrayData& operator=(DynamicArrayData&&) noexcept = delete;

    ~DynamicArrayData() = default;

    void copy_data(DynamicArrayData const& other) noexcept
    {
      start          = other.start;
      finish         = other.finish;
      end_of_storage = other.end_of_storage;
    }

    void swap_data(DynamicArrayData& other) noexcept
    {
      DynamicArrayData tmp;
      tmp.copy_data(*this);
      copy_data(other);
      other.copy_data(tmp);
    }
  };

  using allocator_type = Allocator;

  DynamicArrayBase() noexcept(std::is_nothrow_default_constructible<rebind_allocator_type>::value) =
      default;

  explicit DynamicArrayBase(allocator_type const& allocator) noexcept
    : allocator(allocator)
  {
  }
  explicit DynamicArrayBase(allocator_type&& allocator) noexcept
    : allocator(std::move(allocator))
  {
  }
  DynamicArrayBase(allocator_type&& allocator, DynamicArrayBase&& other) noexcept
    : allocator(std::move(allocator))
    , storage(std::move(other))
  {
  }
  DynamicArrayBase(size_t n, const allocator_type& allocator)
    : allocator(allocator)
  {
    storage.start          = dsac::allocate(n, this->allocator);
    storage.finish         = storage.start;
    storage.end_of_storage = storage.start + n;
  }

  DynamicArrayBase(DynamicArrayBase const&) = delete;
  DynamicArrayBase(DynamicArrayBase&& other) noexcept
    : allocator(std::move(other))
    , storage(std::move(other))
  {
  }

  ~DynamicArrayBase() noexcept
  {
    dsac::deallocate(storage.start, storage.end_of_storage - storage.start, allocator);
  }

  rebind_allocator_type allocator;
  DynamicArrayData      storage;
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
  using base::allocator;
  using base::storage;

public:
  explicit DynamicArray(allocator_type allocator = allocator_type{}) noexcept
    : base(std::move(allocator))
  {
  }

  DynamicArray(std::initializer_list<T> list, allocator_type allocator = allocator_type{})
    : base(std::move(allocator))
  {
    const size_type size   = list.size();
    storage.start          = dsac::allocate(size, allocator);
    storage.end_of_storage = storage.start + size;
    storage.finish = dsac::uninitialized_copy(list.begin(), list.end(), storage.start, allocator);
  }

  DynamicArray(DynamicArray const& other)
    : base(
          other.size(),
          rebind_allocator_traits::select_on_container_copy_construction(other.allocator))
  {
    storage.finish = dsac::uninitialized_copy(other.begin(), other.end(), storage.start, allocator);
  }

  ~DynamicArray() noexcept
  {
    dsac::destroy(storage.start, storage.finish, allocator);
  }

  [[nodiscard]] inline size_type size() const noexcept
  {
    return size_type(storage.finish - storage.start);
  }

  [[nodiscard]] inline size_type capacity() const noexcept
  {
    return size_type(storage.end_of_storage - storage.start);
  }

  void push_back(value_type const& value)
  {
    if (storage.finish != storage.end_of_storage) {
      dsac::construct(storage.finish, allocator, value);
      ++storage.finish;
    }
    else {
      realloc_and_insert(value);
    }
  }

  [[nodiscard]] iterator begin() noexcept
  {
    return iterator(storage.start);
  }

  [[nodiscard]] const_iterator begin() const noexcept
  {
    return const_iterator(storage.start);
  }

  [[nodiscard]] iterator end() noexcept
  {
    return iterator(storage.finish);
  }

  [[nodiscard]] const_iterator end() const noexcept
  {
    return const_iterator(storage.finish);
  }

  [[nodiscard]] reference operator[](size_type n) noexcept
  {
    return *(storage.start + n);
  }

private:
  size_type twice_size(size_type current_size) const
  {
    const size_type max_size = allocator_traits::max_size(allocator);
    if (max_size - current_size < 1U) {
      throw std::range_error{"Not enough memory to allocate"};
    }

    const size_type len = current_size + std::max<size_type>(current_size, 1U);
    return (len < current_size || len > max_size) ? max_size : len;
  }

  template <typename... Args>
  void realloc_and_insert(Args&&... args)
  {
    const size_type new_size   = twice_size(size());
    pointer         new_start  = dsac::allocate(new_size, allocator);
    pointer         new_finish = pointer{};

    try {
      // clang-format off
      dsac::construct(new_start + size(), allocator, std::forward<Args>(args)...);
      new_finish = dsac::uninitialized_move_if_noexcept(storage.start, storage.finish, new_start, allocator);
      // clang-format on
    }
    catch (...) {
      if (!new_finish) {
        allocator_traits::destroy(allocator, new_start + size());
      }
      else {
        dsac::destroy(new_start, new_finish, allocator);
      }

      dsac::deallocate(new_start, new_size, allocator);
      throw;
    }

    dsac::destroy(storage.start, storage.finish, allocator);
    dsac::deallocate(storage.start, capacity(), allocator);

    storage.start          = new_start;
    storage.finish         = ++new_finish;
    storage.end_of_storage = new_start + new_size;
  }
};

template <class T, class Allocator>

bool operator==(const DynamicArray<T, Allocator>& lhs, const DynamicArray<T, Allocator>& rhs)
{
  using std::begin;
  using std::end;
  return std::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

}  // namespace dsac::detail