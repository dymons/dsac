#pragma once

#include <dsac/cache/cache.hpp>
#include <dsac/cache/lru.hpp>
#include <dsac/functional/strong_type.hpp>

namespace dsac {

template <typename Key, typename T>
class lru_k final : public cache_base<Key, T> {
public:
  // Constructors

  /*!
    \brief
        User constructor, constructs a cache with limited amount of space.
  */
  lru_k(std::size_t capacity, std::size_t k);

  // Observers

  auto size() const -> std::size_t final;

  // Modifiers

  auto put(Key key, T value) -> void final;

  auto get(Key const& key) const -> std::optional<T> final;

  /*!
    \brief
        Remove the object from the cache.
  */
  auto erase(Key const& key) -> void final;

  // Unittests

  auto get_history_cache() const -> lru<Key, T>;

  auto get_buffer_cache() const -> lru<Key, T>;

private:
  [[nodiscard]] auto is_full() const noexcept -> bool;

  // The total number of elements that the cache can hold before evict elements by K algorithm
  std::size_t capacity_;

  //
  mutable lru<Key, T> history_cache_;

  //
  mutable lru<Key, T> buffer_cache_;
};

}  // namespace dsac

#define CONCURRENCY_LRU_K_CACHE_HPP
#include <dsac/cache/detail/lru-k-inl.hpp>
#undef CONCURRENCY_LRU_K_CACHE_HPP
