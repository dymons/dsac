#pragma once

#include <dsac/container/detail/construct.hpp>
#include <dsac/container/detail/normal_iterator.hpp>

#include <memory>
#include <stdexcept>

namespace dsac {

template <typename T, typename Allocator>
struct dynamic_array_base {
  using allocator_type        = Allocator;
  using rebind_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<T>;
  using pointer               = typename std::allocator_traits<allocator_type>::pointer;

  struct dynamic_array_data {
    // Constructors

    /*!
      \brief
          Default constructor, constructs an empty dynamic_array_data.
    */
    dynamic_array_data() noexcept;

    /*!
      \brief
          Copy constructor.
    */
    dynamic_array_data(dynamic_array_data const&) = delete;

    /*!
      \brief
          Move constructor.
    */
    dynamic_array_data(dynamic_array_data&& other) noexcept;

    // Destructor

    /*!
      \brief
          Destructor.
    */
    ~dynamic_array_data() = default;

    // Assignment

    /*!
      \brief
          Copy conversion constructor.
    */
    dynamic_array_data& operator=(dynamic_array_data const&) = delete;

    /*!
      \brief
          Move conversion constructor.
    */
    dynamic_array_data& operator=(dynamic_array_data&&) noexcept = delete;

    // Modifiers

    /*!
      \brief
          Delete the current data and copy the new ones from other data.
    */
    void copy_data(dynamic_array_data const& other) noexcept;

    /*!
      \brief
          Exchange current data with data from other.
    */
    void swap_data(dynamic_array_data& other) noexcept;

    /// Pointer to the beginning of the buffer with user data
    pointer start;

    /// Pointer to the ending of the buffer with user data
    pointer finish;

    /// Pointer to the end of the allocated buffer
    pointer end_of_storage;
  };

  // Constructors

  /*!
    \brief
        Default constructor, constructs an empty dynamic_array_base.
  */
  dynamic_array_base() noexcept(std::is_nothrow_default_constructible<rebind_allocator_type>::value) = default;

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined allocator.
  */
  explicit dynamic_array_base(allocator_type const& allocator) noexcept;

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined allocator.
  */
  explicit dynamic_array_base(allocator_type&& allocator) noexcept;

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined number of empty elements.
  */
  explicit dynamic_array_base(size_t n, allocator_type allocator = allocator_type{});

  /*!
    \brief
        Copy constructor.
  */
  dynamic_array_base(dynamic_array_base const&) = delete;

  /*!
    \brief
        Move constructor.
  */
  dynamic_array_base(dynamic_array_base&& other) noexcept;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~dynamic_array_base() noexcept;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  dynamic_array_base& operator=(dynamic_array_base const&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  dynamic_array_base& operator=(dynamic_array_base&&) noexcept = delete;

  /// Policy-based design / Policy for working with memory allocation and release
  rebind_allocator_type allocator_;

  /// User storage
  dynamic_array_data storage_;
};

template <typename T, typename Allocator = std::allocator<T>>
class dynamic_array : protected dynamic_array_base<T, Allocator> {
  using base                    = dynamic_array_base<T, Allocator>;
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
  using iterator               = normal_iterator<pointer, dynamic_array>;
  using const_iterator         = normal_iterator<const_pointer, dynamic_array>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

protected:
  using base::allocator_;
  using base::storage_;

public:
  explicit dynamic_array(allocator_type allocator = allocator_type{}) noexcept;

  explicit dynamic_array(size_type n, value_type const& value = value_type{}, allocator_type alloc = allocator_type{});

  dynamic_array(std::initializer_list<T> list, allocator_type allocator = allocator_type{});

  dynamic_array(dynamic_array const& other);

  dynamic_array(dynamic_array&& other) noexcept = default;

  dynamic_array& operator=(dynamic_array const& x);

  dynamic_array& operator=(dynamic_array&& other) = delete;  // Sorry, unimplemented yet ¯\_(ツ)_/¯

  ~dynamic_array() noexcept;

  [[nodiscard]] inline size_type size() const noexcept;

  [[nodiscard]] inline size_type capacity() const noexcept;

  void push_back(value_type const& value);

  template <typename... Args>
  void emplace_back(Args&&... args);

  [[nodiscard]] iterator begin() noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] iterator end() noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] reference operator[](size_type n) noexcept;

  [[nodiscard]] bool empty() const noexcept;

  void clear() noexcept;

private:
  size_type twice_size(size_type current_size) const;

  template <typename... Args>
  void realloc_and_insert(Args&&... args);
};

template <class T, class Allocator>
bool operator==(const dynamic_array<T, Allocator>& lhs, const dynamic_array<T, Allocator>& rhs) {
  using std::begin;
  using std::end;
  return std::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

}  // namespace dsac

#define CONTAINER_DYNAMIC_ARRAY_HPP
#include <dsac/container/detail/dynamic_array-inl.hpp>
#undef CONTAINER_DYNAMIC_ARRAY_HPP
