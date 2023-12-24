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

    \param key
        A key to be added to the cache
    \param value
        A value to be added to the cache by key

    \par Worst Case Complexity
        Time Complexity \p O(1) and Space Complexity \p O(1)

    \ingroup
        DsacCache

    \code
        auto cache = dsac::lru<int, int>{1z};
        cache.put(1, 1);
        cache.put(2, 2);
    \endcode
  */
  auto put(Key key, T value) -> bool final;

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
