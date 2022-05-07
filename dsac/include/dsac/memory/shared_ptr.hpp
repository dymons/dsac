#pragma once

#include <atomic>

namespace dsac {

namespace detail {

class deleter_base {
public:
  deleter_base()                                   = default;
  deleter_base(const deleter_base&)                = default;
  deleter_base(deleter_base&&) noexcept            = default;
  deleter_base& operator=(const deleter_base&)     = default;
  deleter_base& operator=(deleter_base&&) noexcept = default;
  virtual ~deleter_base()                          = default;
  virtual void operator()(void*)                   = 0;
};

template <typename T>
class deleter : public deleter_base {
public:
  void operator()(void* p) override {
    delete static_cast<T*>(p);
  }
};

}  // namespace detail

template <typename T>
class shared_ptr {
public:
  template <typename U>
  friend class shared_ptr;

  /*!
    \brief
        Default constructor, constructs an empty shared_ptr.
  */
  constexpr shared_ptr() noexcept = default;

  /*!
    \brief
        Constructs an empty shared_ptr.
  */
  constexpr shared_ptr(std::nullptr_t) noexcept {  // NOLINT(google-explicit-constructor)
  }

  /*!
    \brief
        Constructor to wrap raw pointer.
  */
  explicit shared_ptr(T* p)
    : _ptr{p}
    , _ref_count{new std::atomic<long>{1}}
    , _deleter{new detail::deleter<T>()} {
  }

  /*!
    \brief
        Constructor to wrap raw pointer of convertible type.
  */
  template <std::derived_from<T> U>
  explicit shared_ptr(U* p)
    : _ptr{p}
    , _ref_count{new std::atomic<long>{1}}
    , _deleter{new detail::deleter<U>()} {
  }

  /*!
    \brief
        Copy constructor.
  */
  shared_ptr(const shared_ptr& sp) noexcept
    : _ptr{sp._ptr}
    , _ref_count{sp._ref_count}
    , _deleter{sp._deleter} {
    if (_ptr != nullptr) {
      ++(*_ref_count);
    }
  }

  /*!
    \brief
        Copy conversion constructor.
  */
  template <typename U>
  shared_ptr(const shared_ptr<U>& sp) noexcept  // NOLINT(google-explicit-constructor)
    : _ptr{sp._ptr}
    , _ref_count{sp._ref_count}
    , _deleter{sp._deleter} {
    if (_ptr != nullptr) {
      ++(*_ref_count);
    }
  }

  /*!
    \brief
        Move constructor.
  */
  shared_ptr(shared_ptr&& other) noexcept
    : _ptr(other._ptr)
    , _ref_count(other._ref_count)
    , _deleter(other._deleter) {
    other._ptr       = nullptr;
    other._ref_count = nullptr;
  }

  /*!
    \brief
        Move conversion constructor.
  */
  shared_ptr& operator=(shared_ptr&& other) noexcept {
    reset();

    _ptr       = std::exchange(other._ptr, nullptr);
    _ref_count = std::exchange(other._ref_count, nullptr);
    _deleter   = std::exchange(other._deleter, nullptr);

    return *this;
  }

  // Destructor

  /// No side-effect if shared_ptr is empty or use_count() > 1,
  /// otherwise release the resources
  ~shared_ptr() {
    if (_ptr) {
      if (--(*_ref_count) == 0) {
        delete _ref_count;
        (*_deleter)(_ptr);
        delete _deleter;
      }
    }
  }

  // Assignment

  shared_ptr& operator=(const shared_ptr& sp) noexcept {
    /// copy and swap idiom
    shared_ptr tmp{sp};
    tmp.swap(*this);
    return *this;
  }

  // Observers

  /// Dereferences pointer to the managed object
  T& operator*() const noexcept {
    return *_ptr;
  }

  /// Dereferences pointer to the managed object
  T* operator->() const noexcept {
    return _ptr;
  }

  /// Returns the contained pointer
  T* get() const noexcept {
    return _ptr;
  }

  /// Returns use_count (use_count == 0 if shared_ptr is empty)
  long use_count() const noexcept {
    if (_ptr) {
      return *_ref_count;
    } else {
      return 0;
    }
  }

  /// Checks if solely owns the managed object
  bool unique() const noexcept {
    return (use_count() == 1) ? true : false;
  }

  /// Checks if there is an associated managed object
  explicit operator bool() const noexcept {
    return (_ptr) ? true : false;
  }

  // Modifiers

  /// Resets shared_ptr to empty
  void reset() noexcept {
    shared_ptr tmp{};
    tmp.swap(*this);
  }

  /// Resets shared_ptr to wrap raw pointer p
  template <typename U>
  void reset(U* p) {
    shared_ptr tmp{p};
    tmp.swap(*this);
  }

  /// Swap with another shared_ptr
  void swap(shared_ptr& sp) noexcept {
    using std::swap;
    swap(_ptr, sp._ptr);
    swap(_ref_count, sp._ref_count);
    swap(_deleter, sp._deleter);
  }

private:
  T*                    _ptr       = nullptr;  /// contained pointer
  std::atomic<long>*    _ref_count = nullptr;  /// reference counter
  detail::deleter_base* _deleter   = nullptr;  /// deleter
};

// Operator == overloading

template <typename T, typename U>
inline bool operator==(const shared_ptr<T>& sp1, const shared_ptr<U>& sp2) {
  return sp1.get() == sp2.get();
}

template <typename T>
inline bool operator==(const shared_ptr<T>& sp, std::nullptr_t) noexcept {
  return !sp;
}

template <typename T>
inline bool operator==(std::nullptr_t, const shared_ptr<T>& sp) noexcept {
  return !sp;
}

// Operator != overloading

template <typename T, typename U>
inline bool operator!=(const shared_ptr<T>& sp1, const shared_ptr<U>& sp2) {
  return sp1.get() != sp2.get();
}

template <typename T>
inline bool operator!=(const shared_ptr<T>& sp, std::nullptr_t) noexcept {
  return bool{sp};
}

template <typename T>
inline bool operator!=(std::nullptr_t, const shared_ptr<T>& sp) noexcept {
  return bool{sp};
}

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
  return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace dsac
