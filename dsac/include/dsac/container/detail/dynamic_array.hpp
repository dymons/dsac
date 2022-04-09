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

  struct DynamicArrayImpl : public rebind_allocator_type, public DynamicArrayData {
    // clang-format off
    DynamicArrayImpl() noexcept(std::is_nothrow_default_constructible<rebind_allocator_type>::value) = default;
    // clang-format on

    explicit DynamicArrayImpl(rebind_allocator_type const& allocator) noexcept
      : rebind_allocator_type(allocator)
    {
    }
    explicit DynamicArrayImpl(rebind_allocator_type&& allocator) noexcept
      : rebind_allocator_type(std::move(allocator))
    {
    }
    DynamicArrayImpl(rebind_allocator_type&& allocator, DynamicArrayImpl&& other) noexcept
      : rebind_allocator_type(std::move(allocator))
      , DynamicArrayData(std::move(other))
    {
    }

    DynamicArrayImpl(DynamicArrayImpl const&) = delete;
    DynamicArrayImpl(DynamicArrayImpl&& other) noexcept
      : rebind_allocator_type(std::move(other))
      , DynamicArrayData(std::move(other))
    {
    }

    DynamicArrayImpl& operator=(DynamicArrayImpl const&) = delete;
    DynamicArrayImpl& operator=(DynamicArrayImpl&&) noexcept = delete;

    ~DynamicArrayImpl() = default;
  };

public:
  using allocator_type = Allocator;

  DynamicArrayBase() = default;

  explicit DynamicArrayBase(const allocator_type& allocator) noexcept
    : impl_(allocator)
  {
  }

  DynamicArrayBase(size_t n, const allocator_type& allocator)
    : impl_(allocator)
  {
    create_storage(n);
  }

  DynamicArrayBase(const allocator_type& allocator, DynamicArrayBase&& other)
    : impl_(rebind_allocator_type(allocator), std::move(other.impl))
  {
  }

  DynamicArrayBase(DynamicArrayBase&&) noexcept = default;

  ~DynamicArrayBase() noexcept
  {
    deallocate(impl_.start, impl_.end_of_storage - impl_.start);
  }

  DynamicArrayImpl impl_;

protected:
  rebind_allocator_type& get_rebind_allocator() noexcept
  {
    return this->impl_;
  }

  const rebind_allocator_type& get_rebind_allocator() const noexcept
  {
    return this->impl_;
  }

  allocator_type get_allocator() const noexcept
  {
    return allocator_type(get_rebind_allocator());
  }

  [[nodiscard]] pointer allocate(size_t n)
  {
    using allocator = std::allocator_traits<rebind_allocator_type>;
    return n != 0 ? allocator::allocate(impl_, n) : pointer{};
  }

  void deallocate(pointer raw_pointer, size_t n)
  {
    using allocator = std::allocator_traits<rebind_allocator_type>;
    if (raw_pointer) {
      allocator::deallocate(impl_, raw_pointer, n);
    }
  }

  void create_storage(size_t n)
  {
    this->impl.start          = this->allocate(n);
    this->impl.finish         = this->impl.start;
    this->impl.end_of_storage = this->impl.start + n;
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
  using base::deallocate;
  using base::get_rebind_allocator;
  using base::impl_;

public:
  DynamicArray() noexcept = default;

  explicit DynamicArray(const allocator_type& allocator) noexcept
    : base(allocator)
  {
  }

  ~DynamicArray() noexcept
  {
    dsac::destroy(impl_.start, impl_.finish, get_rebind_allocator());
  }

  [[nodiscard]] size_type size() const noexcept
  {
    return size_type(impl_.finish - impl_.start);
  }

  [[nodiscard]] size_type capacity() const noexcept
  {
    return size_type(impl_.end_of_storage - impl_.start);
  }

  void push_back(value_type const& value)
  {
    if (impl_.finish != impl_.end_of_storage) {
      allocator_traits::construct(impl_, impl_.finish, value);
      ++impl_.finish;
    }
    else {
      realloc_insert(end(), value);
    }
  }

  [[nodiscard]] iterator begin() noexcept
  {
    return iterator(impl_.start);
  }

  [[nodiscard]] const_iterator begin() const noexcept
  {
    return const_iterator(impl_.start);
  }

  [[nodiscard]] iterator end() noexcept
  {
    return iterator(impl_.finish);
  }

  [[nodiscard]] const_iterator end() const noexcept
  {
    return const_iterator(impl_.finish);
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
      allocator_traits::construct(impl_, new_start + elements_before, std::forward<Args>(args)...);

      new_finish = dsac::uninitialized_move_if_noexcept(
          impl_.start, position.base(), new_start, get_rebind_allocator());

      ++new_finish;
    }
    catch (...) {
      if (!new_finish) {
        allocator_traits::destroy(impl_, new_start + elements_before);
      }
      else {
        dsac::destroy(new_start, new_finish, get_rebind_allocator());
      }

      this->deallocate(new_start, length);
      throw;
    }

    dsac::destroy(impl_.start, impl_.finish, get_rebind_allocator());
    this->deallocate(impl_.start, impl_.end_of_storage - impl_.start);

    impl_.start          = new_start;
    impl_.finish         = new_finish;
    impl_.end_of_storage = new_start + length;
  }
};

}  // namespace dsac::detail