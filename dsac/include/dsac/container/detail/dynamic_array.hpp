#pragma once

#include <dsac/container/detail/construct.hpp>
#include <dsac/container/detail/normal_iterator.hpp>
#include <memory>
#include <stdexcept>

namespace dsac {

class DynamicArrayException : public std::range_error {
public:
  using std::range_error::range_error;
};

}  // namespace dsac

namespace dsac::detail {

template <typename T, typename Allocator>
class DynamicArrayBase {
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

public:
  using allocator_type = Allocator;

  DynamicArrayBase() noexcept(std::is_nothrow_default_constructible<rebind_allocator_type>::value) =
      default;

  explicit DynamicArrayBase(rebind_allocator_type const& allocator) noexcept
    : allocator(allocator)
  {
  }
  explicit DynamicArrayBase(rebind_allocator_type&& allocator) noexcept
    : allocator(std::move(allocator))
  {
  }
  DynamicArrayBase(rebind_allocator_type&& allocator, DynamicArrayBase&& other) noexcept
    : allocator(std::move(allocator))
    , storage(std::move(other))
  {
  }

  DynamicArrayBase(DynamicArrayBase const&) = delete;
  DynamicArrayBase(DynamicArrayBase&& other) noexcept
    : allocator(std::move(other))
    , storage(std::move(other))
  {
  }

  ~DynamicArrayBase() noexcept
  {
    deallocate(storage.start, storage.end_of_storage - storage.start);
  }

  rebind_allocator_type allocator;
  DynamicArrayData      storage;

protected:
  rebind_allocator_type& get_rebind_allocator() noexcept
  {
    return allocator;
  }

  const rebind_allocator_type& get_rebind_allocator() const noexcept
  {
    return allocator;
  }

  allocator_type get_allocator() const noexcept
  {
    return allocator_type(get_rebind_allocator());
  }

  [[nodiscard]] pointer allocate(size_t n)
  {
    using allocator_traits = std::allocator_traits<rebind_allocator_type>;
    return n != 0 ? allocator_traits::allocate(allocator, n) : pointer{};
  }

  void deallocate(pointer raw_pointer, size_t n)
  {
    using allocator_traits = std::allocator_traits<rebind_allocator_type>;
    if (raw_pointer) {
      allocator_traits::deallocate(allocator, raw_pointer, n);
    }
  }

  void create_storage(size_t n)
  {
    storage.start          = allocate(n);
    storage.finish         = storage.start;
    storage.end_of_storage = storage.start + n;
  }
};

template <typename T, typename Allocator>
class DynamicArray : protected DynamicArrayBase<T, Allocator> {
  using base = DynamicArrayBase<T, Allocator>;

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
  using base::allocate;
  using base::allocator;
  using base::deallocate;
  using base::get_rebind_allocator;
  using base::storage;

public:
  DynamicArray() noexcept = default;

  explicit DynamicArray(const allocator_type& allocator) noexcept
    : base(allocator)
  {
  }

  ~DynamicArray() noexcept
  {
    dsac::destroy(storage.start, storage.finish, get_rebind_allocator());
  }

  [[nodiscard]] size_type size() const noexcept
  {
    return size_type(storage.finish - storage.start);
  }

  [[nodiscard]] size_type capacity() const noexcept
  {
    return size_type(storage.end_of_storage - storage.start);
  }

  void push_back(value_type const& value)
  {
    if (storage.finish != storage.end_of_storage) {
      allocator_traits::construct(allocator, storage.finish, value);
      ++storage.finish;
    }
    else {
      realloc_insert(end(), value);
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

private:
  size_type max_size() const noexcept
  {
    return allocator_traits::max_size(get_rebind_allocator());
  }

  size_type check_len(size_type n, const char* message) const
  {
    if (max_size() - size() < n) {
      throw DynamicArrayException{message};
    }

    const size_type len = size() + std::max(size(), n);
    return (len < size() || len > max_size()) ? max_size() : len;
  }

  template <typename... Args>
  void realloc_insert(iterator position, Args&&... args)
  {
    const size_type length          = check_len(size_type(1U), "DynamicArray::realloc_insert");
    const size_type elements_before = position - begin();
    pointer         new_start       = this->allocate(length);
    pointer         new_finish      = pointer{};

    try {
      allocator_traits::construct(
          allocator, new_start + elements_before, std::forward<Args>(args)...);

      new_finish = dsac::uninitialized_move_if_noexcept(
          storage.start, position.base(), new_start, get_rebind_allocator());

      ++new_finish;
    }
    catch (...) {
      if (!new_finish) {
        allocator_traits::destroy(allocator, new_start + elements_before);
      }
      else {
        dsac::destroy(new_start, new_finish, get_rebind_allocator());
      }

      this->deallocate(new_start, length);
      throw;
    }

    dsac::destroy(storage.start, storage.finish, get_rebind_allocator());
    this->deallocate(storage.start, storage.end_of_storage - storage.start);

    storage.start          = new_start;
    storage.finish         = new_finish;
    storage.end_of_storage = new_start + length;
  }
};

}  // namespace dsac::detail