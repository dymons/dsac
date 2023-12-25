#pragma once

namespace dsac {

template <typename Key, typename Value>
class lru_k {
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

  auto get(Key const& key) const -> Value*;

private:
//  [[nodiscard]] auto is_full() const noexcept -> bool;
//
//  // The total number of elements that the cache can hold before evict elements by K algorithm
//  std::size_t capacity_;
//
//  //
//  mutable lru<Key, T> history_cache_;
//
//  //
//  mutable lru<Key, T> buffer_cache_;
};

}  // namespace dsac

#define CONCURRENCY_LRU_K_CACHE_HPP
#include <dsac/cache/detail/lru-k-inl.hpp>
#undef CONCURRENCY_LRU_K_CACHE_HPP
