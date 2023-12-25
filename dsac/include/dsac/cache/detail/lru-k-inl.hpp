#ifndef CONCURRENCY_LRU_K_CACHE_HPP
#error This file may only be included from lru-k.hpp
#endif

namespace dsac {

template <typename Key, typename T>
lru_k<Key, T>::lru_k(std::size_t const capacity, std::size_t const k)
  : capacity_(capacity)
  , history_cache_(capacity_)
  , buffer_cache_(k) {
}

template <typename Key, typename T>
auto lru_k<Key, T>::put(Key key, T value) -> void {
  if (history_cache_.get(key)) {
//    history_cache_.erase(key);
    buffer_cache_.put(std::move(key), std::move(value));
    return;
  }

  if (capacity_ == buffer_cache_.size()) {
    buffer_cache_.put(std::move(key), std::move(value));
    return;
  }

  if (is_full()) {
    // history_cache_.try_remove_latest();
  }

  history_cache_.put(key, value);
}

template <typename Key, typename T>
auto lru_k<Key, T>::get(Key const& key) const -> std::optional<T> {
  if (auto value = buffer_cache_.get(key); nullptr != value) {
    return *value;
  }

  if (auto value = history_cache_.get(key); nullptr != value) {
//    history_cache_.erase(key);
    buffer_cache_.put(key, *value);
    return *value;
  }

  return std::nullopt;
}

template <typename Key, typename T>
auto lru_k<Key, T>::erase(Key const& key) -> void {
//  history_cache_.erase(key);
//  buffer_cache_.erase(key);
}

template <typename Key, typename T>
auto lru_k<Key, T>::is_full() const noexcept -> bool {
  return (capacity_ - buffer_cache_.size()) == history_cache_.size();
}

template <typename Key, typename T>
auto lru_k<Key, T>::size() const -> std::size_t {
  return history_cache_.size() + buffer_cache_.size();
}

template <typename Key, typename T>
auto lru_k<Key, T>::get_history_cache() const -> lru<Key, T> {
  return history_cache_;
}

template <typename Key, typename T>
auto lru_k<Key, T>::get_buffer_cache() const -> lru<Key, T> {
  return buffer_cache_;
}

}  // namespace dsac