#pragma once

#include <list>
#include <unordered_map>

#include <dsac/cache/cache.hpp>

namespace dsac {

template <typename Key, typename T>
class lru final : public cache_base<Key, T> {
public:
  // Constructors

  /*!
    \brief
        User constructor, constructs a cache with limited amount of space.
  */
  explicit lru(std::size_t capacity);

  // Modifiers

  /*!
    \brief
        Add a new value to the cache.
  */
  auto put(Key key, T value) -> void final;

  /*!
    \brief
        Extract the object from the cache.
  */
  auto get(Key const& key) const -> std::optional<T> final;

  /*!
    \brief
        Returns the number of elements in the cache.
  */
  auto size() const -> std::size_t final;

private:
  // The total number of elements that the cache can hold before evict latest elements
  std::size_t capacity_;

  // The store for values
  std::list<T> cache_;

  // The store to lookup values by key at the cache
  std::unordered_map<Key, typename decltype(cache_)::iterator> hash_;
};

}  // namespace dsac

#define CONCURRENCY_LRU_CACHE_HPP
#include <dsac/cache/detail/lru-inl.hpp>
#undef CONCURRENCY_LRU_CACHE_HPP
