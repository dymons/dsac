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
    : ptr_{p}
    , ref_count_{new std::atomic<long>{1}}
    , deleter_{new detail::deleter<T>()} {
  }

  /*!
    \brief
        Constructor to wrap raw pointer of convertible type.
  */
  template <std::derived_from<T> U>
  explicit shared_ptr(U* p)
    : ptr_{p}
    , ref_count_{new std::atomic<long>{1}}
    , deleter_{new detail::deleter<U>()} {
  }

  /*!
    \brief
        Copy constructor.
  */
  shared_ptr(const shared_ptr& sp) noexcept
    : ptr_{sp.ptr_}
    , ref_count_{sp.ref_count_}
    , deleter_{sp.deleter_} {
    if (ptr_ != nullptr) {
      ++(*ref_count_);
    }
  }

  /*!
    \brief
        Copy conversion constructor.
  */
  template <typename U>
  shared_ptr(const shared_ptr<U>& sp) noexcept  // NOLINT(google-explicit-constructor)
    : ptr_{sp.ptr_}
    , ref_count_{sp.ref_count_}
    , deleter_{sp.deleter_} {
    if (ptr_ != nullptr) {
      ++(*ref_count_);
    }
  }

  /*!
    \brief
        Move constructor.
  */
  shared_ptr(shared_ptr&& other) noexcept
    : ptr_(other.ptr_)
    , ref_count_(other.ref_count_)
    , deleter_(other.deleter_) {
    other.ptr_       = nullptr;
    other.ref_count_ = nullptr;
  }

  /*!
    \brief
        Move conversion constructor.
  */
  shared_ptr& operator=(shared_ptr&& other) noexcept {
    reset();

    ptr_       = std::exchange(other.ptr_, nullptr);
    ref_count_ = std::exchange(other.ref_count_, nullptr);
    deleter_   = std::exchange(other.deleter_, nullptr);

    return *this;
  }

  // Destructor

  /// No side-effect if shared_ptr is empty or use_count() > 1,
  /// otherwise release the resources
  ~shared_ptr() {
    if (ptr_) {
      if (--(*ref_count_) == 0) {
        delete ref_count_;
        (*deleter_)(ptr_);
        delete deleter_;
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
    return *ptr_;
  }

  /// Dereferences pointer to the managed object
  T* operator->() const noexcept {
    return ptr_;
  }

  /// Returns the contained pointer
  T* get() const noexcept {
    return ptr_;
  }

  /// Returns use_count (use_count == 0 if shared_ptr is empty)
  long use_count() const noexcept {
    if (ptr_) {
      return *ref_count_;
    }
    return 0;
  }

  /// Checks if solely owns the managed object
  bool unique() const noexcept {
    return (use_count() == 1);
  }

  /// Checks if there is an associated managed object
  explicit operator bool() const noexcept {
    return (ptr_ != nullptr);
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
    swap(ptr_, sp.ptr_);
    swap(ref_count_, sp.ref_count_);
    swap(deleter_, sp.deleter_);
  }

private:
  T*                    ptr_       = nullptr;  /// contained pointer
  std::atomic<long>*    ref_count_ = nullptr;  /// reference counter
  detail::deleter_base* deleter_   = nullptr;  /// deleter
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
