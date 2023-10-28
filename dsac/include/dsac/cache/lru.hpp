#pragma once

#include <list>
#include <unordered_map>

#include <dsac/cache/cache.hpp>

namespace dsac {

template <typename Key, typename Value>
class lru final : public cache_base<Key, Value> {
public:
  explicit lru(std::size_t capacity);

  auto put(Key key, Value value) -> void final;

  auto get(Key const& key) -> std::optional<Value> final;

  auto size() const -> std::size_t final;

private:
  std::size_t                                                  capacity_;
  std::list<Value>                                             cache_;
  std::unordered_map<Key, typename decltype(cache_)::iterator> hash_;
};

}  // namespace dsac

#define CONCURRENCY_LRU_CACHE_HPP
#include <dsac/cache/detail/lru-inl.hpp>
#undef CONCURRENCY_LRU_CACHE_HPP
