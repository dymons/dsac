#ifndef CONCURRENCY_LRU_CACHE_HPP
#error This file may only be included from lru.hpp
#endif

namespace dsac {

template <typename Key, typename Value>
lru<Key, Value>::lru(std::size_t cache_size)
  : cache_size_(cache_size) {
}

template <typename Key, typename Value>
auto lru<Key, Value>::put(Key key, Value value) -> void {
  auto hit = hash_.find(key);
  if (hit == hash_.end()) {
    if (size() == cache_size_) {
      hash_.erase(cache_.back());
      cache_.pop_back();
    }

    cache_.push_front(std::move(value));
    hash_[std::move(key)] = cache_.begin();

    return;
  }

  if (hit->second != cache_.begin()) {
    cache_.splice(cache_.begin(), cache_, hit->second, std::next(hit->second));
  }
}

template <typename Key, typename Value>
auto lru<Key, Value>::get(Key const& key) -> std::optional<Value> {
  auto hit = hash_.find(key);
  if (hit == hash_.end()) {
    return std::nullopt;
  }

  return *hit->second;
}

template <typename Key, typename Value>
auto lru<Key, Value>::size() const -> std::size_t {
  return cache_.size();
}

}  // namespace dsac
