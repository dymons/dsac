#pragma once

#include <optional>
#include <functional>
#include <unordered_set>

#include <dsac/container/intrusive/list.hpp>

namespace dsac {

template <typename Key, typename Value>
class lru {
public:
  // Constructors

  /*!
    \brief
        User constructor, constructs a cache with limited amount of space.
  */
  explicit lru(std::size_t capacity);

  // Observers

  /*!
    \brief
        Returns the number of elements in the cache.
  */
  auto size() const -> std::size_t;

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
  auto put(Key key, Value value) -> void;

  /*!
    \brief
        Extract the object from the cache.
  */
  auto get(Key const& key) const -> std::optional<Value>;

  /*!
    \brief
        Remove the object from the cache.
  */
  auto erase(Key const& key) -> void;

private:
  // The total number of elements that the cache can hold before evict latest elements
  std::size_t capacity_;

  struct item : intrusive::list_node_base<item> {
    explicit item(const Key& key, const Value& value = Value())
      : key(key)
      , value(value) {
    }

    bool operator==(const item& other) const {
      return key == other.key;
    }

    Key   key;
    Value value;

    struct hash {
      size_t operator()(const item& item) const {
        return std::hash<Key>{}(item.key);
      }
    };
  };

  // The store for values
  using cache = intrusive::list<item>;
  mutable cache cache_;

  // The store to lookup values by key at the cache
  using index = std::unordered_set<item, typename item::hash>;
  mutable std::unordered_set<item, typename item::hash> index_;

  auto promote(const index::iterator& it) const -> void;
};

}  // namespace dsac

#define CONCURRENCY_LRU_CACHE_HPP
#include <dsac/cache/detail/lru-inl.hpp>
#undef CONCURRENCY_LRU_CACHE_HPP
