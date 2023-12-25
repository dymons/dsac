#ifndef CONCURRENCY_LRU_K_CACHE_HPP
#error This file may only be included from lru-k.hpp
#endif

namespace dsac {
//
template <typename Key, typename Value>
lru_k<Key, Value>::lru_k(std::size_t const capacity, std::size_t const k)
  : capacity_(capacity)
  , k_(k) {
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::put(Key key, Value value) -> void {
  auto tmp_item = item{std::move(key), std::move(value)};

  auto history_hit = history_index_.find(tmp_item);
  if (history_hit == history_index_.end()) {
    auto buffer_hit = buffer_index_.find(tmp_item);

    if (buffer_hit == buffer_index_.end()) {
      // Case 1. What: Value is new
      //         Then: Add value to the history cache

      history_hit = history_index_.insert(std::move(tmp_item)).first;
      history_cache_.push_front(const_cast<item*>(&*history_hit));

      return;
    } else {
      // Case 2. What: Value is already exist at buffer cache
      //         Then: Promote value to the top of buffer cache

      promote(buffer_hit);

      return;
    }
  } else {

    // Case 3. What: Value is already exist at history cache
    //         Then: Move value from history cache to the buffer cache



  }
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::promote(const index::iterator& it) const -> void {
  auto tmp_item = const_cast<item*>(&*it);
  tmp_item->detach();
  buffer_cache_.push_front(tmp_item);
}

//
// template <typename Key, typename T>
// auto lru_k<Key, T>::get(Key const& key) const -> std::optional<T> {
//  if (auto value = buffer_cache_.get(key); nullptr != value) {
//    return *value;
//  }
//
//  if (auto value = history_cache_.get(key); nullptr != value) {
////    history_cache_.erase(key);
//    buffer_cache_.put(key, *value);
//    return *value;
//  }
//
//  return std::nullopt;
//}
//
// template <typename Key, typename T>
// auto lru_k<Key, T>::erase(Key const& key) -> void {
////  history_cache_.erase(key);
////  buffer_cache_.erase(key);
//}
//
//
// template <typename Key, typename T>
// auto lru_k<Key, T>::size() const -> std::size_t {
//  return history_cache_.size() + buffer_cache_.size();
//}

}  // namespace dsac