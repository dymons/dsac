#ifndef CONCURRENCY_LRU_K_CACHE_HPP
#error This file may only be included from lru-k.hpp
#endif

namespace dsac {

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

      self::pop_at_history_cache_while_overflow();

      history_hit = history_index_.insert(std::move(tmp_item)).first;
      history_cache_.push_front(const_cast<item*>(&*history_hit));

      return;
    } else {
      // Case 2. What: Value is already exist at buffer cache
      //         Then: Promote value to the top of buffer cache

      self::promote(buffer_hit);

      return;
    }
  }

  // Case 3. What: Value is already exist at history cache
  //         Then: Move value from history cache to the buffer cache

  self::pop_at_buffer_cache_while_overflow();
  self::move_to_buffer_cache_with_promote(history_hit);

  return;
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::size() const -> std::size_t {
  return history_index_.size() + buffer_index_.size();
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::get(Key const& key) -> Value* {
  auto tmp_item = item{std::move(key)};

  auto history_hit = history_index_.find(tmp_item);
  if (history_hit == history_index_.end()) {
    auto buffer_hit = buffer_index_.find(tmp_item);
    if (buffer_hit == buffer_index_.end()) {
      // Case 1. What: Not found key at history cache and buffer cache
      //         Then: Do nothing

      return nullptr;
    } else {
      // Case 2. What: Key found at buffer cache
      //         Then: Promote key at the buffer cache and return value

      self::promote(buffer_hit);

      return &const_cast<Value&>(buffer_hit->value);
    }
  }

  // Case 3. What: Key found at history cache
  //         Then: Move value from history cache to the buffer cache
  //               and return value

  auto buffer_hit = self::move_to_buffer_cache_with_promote(history_hit);

  return &const_cast<Value&>(buffer_hit->value);
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::promote(const index::iterator& it) -> void {
  auto tmp_item = const_cast<item*>(&*it);
  tmp_item->detach();
  buffer_cache_.push_front(tmp_item);
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::move_to_buffer_cache_with_promote(const index::iterator& it) -> index::iterator {
  const_cast<item*>(&*it)->detach();

  auto buffer_hit = buffer_index_.insert(std::move(*it)).first;
  buffer_cache_.push_front(const_cast<item*>(&*buffer_hit));

  history_index_.erase(it);

  return buffer_hit;
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::pop_at_history_cache_while_overflow() -> void {
  auto const is_overflow = (capacity_ - history_index_.size() - buffer_index_.size()) == 0z;
  if (not is_overflow) {
    return;
  }

  auto* pop_item = history_cache_.pop_back();
  pop_item->detach();

  history_index_.erase(*pop_item);
}

template <typename Key, typename Value>
auto lru_k<Key, Value>::pop_at_buffer_cache_while_overflow() -> void {
  auto const is_overflow = (k_ - buffer_index_.size()) == 0z;
  if (not is_overflow) {
    return;
  }

  auto* pop_item = buffer_cache_.pop_back();
  pop_item->detach();

  buffer_index_.erase(*pop_item);
}

}  // namespace dsac