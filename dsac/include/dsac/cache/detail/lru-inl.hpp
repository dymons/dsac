#ifndef CONCURRENCY_LRU_CACHE_HPP
#error This file may only be included from lru.hpp
#endif

namespace dsac {

template <typename Key, typename T>
lru<Key, T>::lru(std::size_t capacity)
  : capacity_(capacity) {
}

template <typename Key, typename T>
auto lru<Key, T>::put(Key key, T value) -> bool {
  auto hit = hash_.find(key);
  if (hit == hash_.end()) {
    if (size() == capacity_) {
      hash_.erase(cache_.back());
      cache_.pop_back();
    }

    cache_.push_front(std::move(value));
    hash_[std::move(key)] = cache_.begin();

    return true;
  }

  if (hit->second != cache_.begin()) {
    cache_.splice(cache_.begin(), cache_, hit->second, std::next(hit->second));
  }

  return true;
}

template <typename Key, typename T>
auto lru<Key, T>::get(Key const& key) const -> std::optional<T> {
  auto hit = hash_.find(key);
  if (hit == hash_.end()) {
    return std::nullopt;
  }

  return *hit->second;
}

template <typename Key, typename T>
auto lru<Key, T>::pin(Key const& key) -> void {

}

template <typename Key, typename T>
auto lru<Key, T>::unpin(Key const& key) -> void {

}

template <typename Key, typename T>
auto lru<Key, T>::size() const -> std::size_t {
  return cache_.size();
}

}  // namespace dsac
