#pragma once

#include <unordered_set>

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
  */
  lru_k(std::size_t capacity, std::size_t k);

  // Observers

  auto size() const -> std::size_t;

  // Modifiers

  auto put(Key key, Value value) -> void;

  auto get(Key const& key) -> Value*;

  // Testsuite

  [[nodiscard]] auto get_history_index_for_testsuite() const {
    return history_index_;
  }
  [[nodiscard]] auto get_buffer_index_for_testsuite() const {
    return buffer_index_;
  }

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
