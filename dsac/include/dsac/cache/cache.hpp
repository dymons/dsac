#pragma once

#include <optional>

namespace dsac {

template <typename Key, typename Value>
class cache_base {
public:
  // Constructors

  /*!
    \brief
        Default constructor.
  */
  cache_base() = default;

  /*!
    \brief
        Move constructor.
  */
  cache_base(cache_base&&) = default;

  /*!
    \brief
        Copy constructor.
  */
  cache_base(const cache_base&) = default;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  virtual ~cache_base() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  cache_base& operator=(const cache_base&) = default;

  /*!
    \brief
        Move conversion constructor.
  */
  cache_base& operator=(cache_base&&) = default;

  // Observers

  /*!
    \brief
        Return the current size of the cache.
  */
  virtual auto size() const -> std::size_t = 0;

  // Modifiers

  /*!
    \brief
        Put a new key/value to the cache.
  */
  virtual auto put(Key key, Value value) -> void = 0;

  /*!
    \brief
        Get value from the cache by key.
  */
  virtual auto get(Key const& key) const -> std::optional<Value> = 0;

  /*!
    \brief
        Remove value by key. If the key does not exist, nothing happens.
  */
  virtual auto erase(Key const& key) -> void = 0;
};

}  // namespace dsac
