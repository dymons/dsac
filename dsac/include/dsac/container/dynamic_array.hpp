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
  // Constructors

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined allocator.
  */
  explicit dynamic_array(allocator_type allocator = allocator_type{}) noexcept;

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined number of empty elements.
  */
  explicit dynamic_array(size_type n, value_type const& value = value_type{}, allocator_type alloc = allocator_type{});

  /*!
    \brief
        User constructor, constructs an dynamic_array_base with predefined number of elements.
  */
  dynamic_array(std::initializer_list<T> list, allocator_type allocator = allocator_type{});

  /*!
    \brief
        Copy constructor.
  */
  dynamic_array(dynamic_array const& other);

  /*!
    \brief
        Move constructor.
  */
  dynamic_array(dynamic_array&& other) noexcept = default;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~dynamic_array() noexcept;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  dynamic_array& operator=(dynamic_array const& other);

  /*!
    \brief
        Move conversion constructor.
  */
  dynamic_array& operator=(dynamic_array&& other) = delete;  // Sorry, unimplemented yet ¯\_(ツ)_/¯

  // Observers

  /*!
    \brief
        Returns a read/write iterator that points to the first element in the dynamic_array.
        Iteration is done in ordinary element order.
  */
  [[nodiscard]] iterator begin() noexcept;

  /*!
    \brief
        Returns a read-only (constant) iterator that points to the first element in the dynamic_array.
        Iteration is done in ordinary element order.
  */
  [[nodiscard]] const_iterator begin() const noexcept;

  /*!
    \brief
        Returns a read/write iterator that points one past the last element in the dynamic_array.
        Iteration is done in ordinary element order.
  */
  [[nodiscard]] iterator end() noexcept;

  /*!
    \brief
        Returns a read-only (constant) iterator that points one past the last element in the dynamic_array.
        Iteration is done in ordinary element order.
  */
  [[nodiscard]] const_iterator end() const noexcept;

  /*!
    \brief
        Returns true if the dynamic_array is empty. (Thus begin() would equal end().)
  */
  [[nodiscard]] bool empty() const noexcept;

  /*!
    \brief
        Returns the number of elements in the dynamic_array.
  */
  [[nodiscard]] size_type size() const noexcept;

  /*!
    \brief
        Returns the total number of elements that the dynamic_array can hold before needing to allocate more memory.
  */
  [[nodiscard]] size_type capacity() const noexcept;

  // Modifiers

  /*!
    \brief
        Add data to the end of the dynamic_array.

    \param value
        Data to be added

    \throw std::range_error
        During reallocation, there may not be enough memory to double the size of the vector

    \par Amortized Case Complexity:
        This is a typical stack operation. The function creates an element at the end of the %vector and assigns the
        given data to it.  Due to the nature of a %vector this operation can be done in constant time if the
        dynamic_array has preallocated space available.
        Amortized Time Complexity \p O(1) and Space Complexity \p O(1)

    \ingroup
        DsacContainer

    \code
        auto user_data = dsac::dynamic_array<int>{};
        user_data.push_back(1);
        user_data.push_back(2);
    \endcode
  */
  void push_back(value_type const& value);
  void push_back(value_type&& value);

  /*!
    \brief
        Add data to the end of the dynamic_array.

    \param value
        Data to be added

    \throw std::range_error
        During reallocation, there may not be enough memory to double the size of the vector

    \par Amortized Case Complexity:
        This is a typical stack operation. The function creates an element at the end of the %vector and assigns the
        given data to it.  Due to the nature of a %vector this operation can be done in constant time if the
        dynamic_array has preallocated space available.
        Amortized Time Complexity \p O(1) and Space Complexity \p O(1)

    \ingroup
        DsacContainer

    \code
        struct color {
          color(std::uint8_t r, std::uint8_t g, std::uint8_t b) {...}
        };
        auto colors = dsac::dynamic_array<color>{};
        colors.emplace_back(255,255,255);
        colors.emplace_back(0,255,0);
    \endcode
  */
  template <typename... Args>
  void emplace_back(Args&&... args);

  /*!
    \brief
        Erases all the elements.  Note that this function only erases the elements, and that if the elements themselves
        are pointers, the pointed-to memory is not touched in any way.  Managing the pointer is the user's
        responsibility.
  */
  void clear() noexcept;

  /*!
    \brief
        Add data to the end of the dynamic_array.

    \param value
        Data to be added

    \par Worst Case Complexity:
        Time Complexity \p O(1) and Space Complexity \p O(1)

    \ingroup
        DsacContainer

    \code
        auto user_data = dsac::dynamic_array<int>{1,2,3,4,5,6,7,8};
        for (std::size_type i{}; i < user_data.size(); ++i) {
          do_something(user_data[i]);
        }
    \endcode
  */
  [[nodiscard]] reference operator[](size_type n) noexcept;

  /*!
    \brief
        Attempt to preallocate enough memory for specified number of elements.

    \param n
        Number of elements required

    \throw std::range_error
        During reallocation, there may not be enough memory to allocate

    \par Worst Case Complexity:
        Amortized Time Complexity \p O(N) and Space Complexity \p O(N)

    \ingroup
        DsacContainer

    \code
        struct point {...};
        auto triangle = dsac::dynamic_array<point>{};
        triangle.reserve(3);
    \endcode
  */
  void reserve(size_type n);

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
