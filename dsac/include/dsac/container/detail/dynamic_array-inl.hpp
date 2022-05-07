#ifndef CONTAINER_DYNAMIC_ARRAY_HPP
#error This file may only be included from dynamic_array.hpp
#endif

namespace dsac {

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_data::dynamic_array_data() noexcept
  : start(nullptr)
  , finish(nullptr)
  , end_of_storage(nullptr) {
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_data::dynamic_array_data(dynamic_array_data&& other) noexcept
  : start(other.start)
  , finish(other.finish)
  , end_of_storage(other.end_of_storage) {
  other.start = other.finish = other.end_of_storage = pointer{};
}

template <typename T, typename Allocator>
void dynamic_array_base<T, Allocator>::dynamic_array_data::copy_data(dynamic_array_data const& other) noexcept {
  start          = other.start;
  finish         = other.finish;
  end_of_storage = other.end_of_storage;
}

template <typename T, typename Allocator>
void dynamic_array_base<T, Allocator>::dynamic_array_data::swap_data(dynamic_array_data& other) noexcept {
  dynamic_array_data tmp;
  tmp.copy_data(*this);
  copy_data(other);
  other.copy_data(tmp);
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_base(allocator_type const& allocator) noexcept
  : allocator_(allocator) {
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_base(allocator_type&& allocator) noexcept
  : allocator_(std::move(allocator)) {
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_base(size_t n, allocator_type allocator)
  : allocator_(std::move(allocator)) {
  storage_.start          = dsac::allocate(n, allocator_);
  storage_.finish         = storage_.start;
  storage_.end_of_storage = storage_.start + n;
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::dynamic_array_base(dynamic_array_base&& other) noexcept
  : allocator_(std::move(other.allocator_))
  , storage_(std::move(other.storage_)) {
}

template <typename T, typename Allocator>
dynamic_array_base<T, Allocator>::~dynamic_array_base() noexcept {
  dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>::dynamic_array(allocator_type allocator) noexcept
  : base(std::move(allocator)) {
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>::dynamic_array(size_type n, value_type const& value, allocator_type alloc)
  : base(n, std::move(alloc)) {
  dsac::uninitialized_fill_n(storage_.start, n, value, allocator_);
  storage_.finish = storage_.end_of_storage;
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>::dynamic_array(std::initializer_list<T> list, allocator_type allocator)
  : base(std::move(allocator)) {
  const size_type size    = list.size();
  storage_.start          = dsac::allocate(size, allocator_);
  storage_.end_of_storage = storage_.start + size;
  storage_.finish         = dsac::uninitialized_copy(list.begin(), list.end(), storage_.start, allocator_);
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>::dynamic_array(dynamic_array const& other)
  : base(other.size(), rebind_allocator_traits::select_on_container_copy_construction(other.allocator_)) {
  storage_.finish = dsac::uninitialized_copy(other.begin(), other.end(), storage_.start, allocator_);
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>& dynamic_array<T, Allocator>::operator=(dynamic_array const& other) {
  if (&other != this) {
    if (allocator_traits::propagate_on_container_copy_assignment::value) {
      bool const is_always_equal     = allocator_traits::is_always_equal::value;
      bool const is_allocators_equal = allocator_ == other.allocator_;
      if (is_always_equal && !is_allocators_equal) {
        clear();
        dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
        storage_.start          = nullptr;
        storage_.finish         = nullptr;
        storage_.end_of_storage = nullptr;
      }
      dsac::alloc_on_copy(allocator_, other.allocator_);
    }

    const size_type xsize = other.size();
    if (xsize > capacity()) {
      pointer tmp = dsac::allocate_and_copy(xsize, other.begin(), other.end(), allocator_);
      dsac::destroy(storage_.start, storage_.finish, allocator_);
      dsac::deallocate(storage_.start, storage_.end_of_storage - storage_.start, allocator_);
      storage_.start          = tmp;
      storage_.end_of_storage = storage_.start + xsize;
    } else if (size() >= xsize) {
      dsac::destroy(std::copy(other.begin(), other.end(), begin()), end(), allocator_);
    } else {
      std::copy(other.storage_.start, other.storage_.start + size(), storage_.start);
      dsac::uninitialized_copy(other.storage_.start + size(), other.storage_.finish, storage_.finish, allocator_);
    }
    storage_.finish = storage_.start + xsize;
  }
  return *this;
}

template <typename T, typename Allocator>
dynamic_array<T, Allocator>::~dynamic_array() noexcept {
  dsac::destroy(storage_.start, storage_.finish, allocator_);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::size_type dynamic_array<T, Allocator>::size() const noexcept {
  return size_type(storage_.finish - storage_.start);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::size_type dynamic_array<T, Allocator>::capacity() const noexcept {
  return size_type(storage_.end_of_storage - storage_.start);
}

template <typename T, typename Allocator>
void dynamic_array<T, Allocator>::push_back(value_type const& value) {
  if (storage_.finish != storage_.end_of_storage) {
    dsac::construct(storage_.finish, allocator_, value);
    ++storage_.finish;
  } else {
    realloc_and_insert(value);
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void dynamic_array<T, Allocator>::emplace_back(Args&&... args) {
  if (storage_.finish != storage_.end_of_storage) {
    dsac::construct(storage_.finish, allocator_, T{std::forward<Args>(args)...});
    ++storage_.finish;
  } else {
    realloc_and_insert(std::forward<Args>(args)...);
  }
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::iterator dynamic_array<T, Allocator>::begin() noexcept {
  return iterator(storage_.start);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::const_iterator dynamic_array<T, Allocator>::begin() const noexcept {
  return const_iterator(storage_.start);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::iterator dynamic_array<T, Allocator>::end() noexcept {
  return iterator(storage_.finish);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::const_iterator dynamic_array<T, Allocator>::end() const noexcept {
  return const_iterator(storage_.finish);
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::reference dynamic_array<T, Allocator>::operator[](size_type n) noexcept {
  return *(storage_.start + n);
}

template <typename T, typename Allocator>
bool dynamic_array<T, Allocator>::empty() const noexcept {
  return begin() == end();
}

template <typename T, typename Allocator>
void dynamic_array<T, Allocator>::clear() noexcept {
  dsac::destroy(storage_.start, storage_.finish, allocator_);
  storage_.finish = storage_.start;
}

template <typename T, typename Allocator>
typename dynamic_array<T, Allocator>::size_type dynamic_array<T, Allocator>::twice_size(size_type current_size) const {
  const size_type max_size = allocator_traits::max_size(allocator_);
  if (max_size - current_size < 1U) {
    throw std::range_error{"Not enough memory to allocate"};
  }

  const size_type len = current_size + std::max<size_type>(current_size, 1U);
  return (len < current_size || len > max_size) ? max_size : len;
}

template <typename T, typename Allocator>
template <typename... Args>
void dynamic_array<T, Allocator>::realloc_and_insert(Args&&... args) {
  const size_type new_size   = twice_size(size());
  pointer         new_start  = dsac::allocate(new_size, allocator_);
  pointer         new_finish = pointer{};

  try {
    dsac::construct(new_start + size(), allocator_, T{std::forward<Args>(args)...});
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

}  // namespace dsac
