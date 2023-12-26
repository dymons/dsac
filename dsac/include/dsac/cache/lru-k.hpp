#pragma once

#include <unordered_set>

#include <dsac/common/macros.h>
#include <dsac/container/intrusive/list.hpp>

namespace dsac {

template <typename Key, typename Value>
class lru_k {
  using self = lru_k;

public:
  // Constructors

  /*!
    \brief
        User constructor, constructs a cache with limited amount of space.

    \param capacity
        The maximum number of values the lru_k will be required to store
    \param k
        The maximum number of values the lru_k will be required to store by K-distance.
  */
  lru_k(std::size_t capacity, std::size_t k);

  /*!
    \brief
        Cache is not copyable and not movable.
  */
  DISALLOW_COPY_AND_MOVE(lru_k);

  // Observers

  /*!
    \brief
        Return LRU-K size, which tracks the number of value at the cache.
  */
  auto size() const -> std::size_t;

  // Modifiers

  /*!
    \brief
        Create a new entry for access if KV has not been seen before.

        If KV is already exist, value will not be updated.
        If KV is exist at history cache, then move KV to buffer cache with promote.
        If KV is exist at buffer cache, then promote.

    \param key
        A new key to store the cache
    \param value
        A new value to store the cache
  */
  auto put(Key key, Value value) -> void;

  /*!
    \brief
        Find KV at cache.

        If KV is not exist, return nullptr.
        If KV is exist at history cache, then move KV to buffer cache with promote and return value.
        If KV is exist at buffer cache, then promote and return value.

     \param key
        The key that will be used to search in the cache
  */
  auto get(Key const& key) -> Value*;

private:
  struct item;
  using cache = intrusive::list<item>;
  using index = std::unordered_set<item, typename item::hash>;

  auto promote(const index::iterator& it) -> void;

  auto move_to_buffer_cache_with_promote(const index::iterator& it) -> index::iterator;

  auto pop_at_history_cache_while_overflow() -> void;

  auto pop_at_buffer_cache_while_overflow() -> void;

  struct item final : intrusive::list_node_base<item> {
    explicit item(const Key& key, const Value& value = Value())
      : key(key)
      , value(value) {
    }

    bool operator==(const item& other) const {
      return key == other.key;
    }

    struct hash {
      size_t operator()(const item& item) const {
        return std::hash<Key>{}(item.key);
      }
    };

    Key   key;
    Value value;
  };

  std::size_t capacity_;
  std::size_t k_;

  cache history_cache_;
  index history_index_;

  cache buffer_cache_;
  index buffer_index_;
};

}  // namespace dsac

#define CONCURRENCY_LRU_K_CACHE_HPP
#include <dsac/cache/detail/lru-k-inl.hpp>
#undef CONCURRENCY_LRU_K_CACHE_HPP
