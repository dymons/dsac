#ifndef CONCURRENCY_LRU_CACHE_HPP
#error This file may only be included from lru.hpp
#endif

namespace dsac {

template <typename Key, typename T>
lru<Key, T>::lru(const std::size_t capacity)
  : capacity_(capacity) {
}

template <typename Key, typename Value>
auto lru<Key, Value>::put(Key key, Value value) -> void {
  auto tmp_item = item{std::move(key), std::move(value)};

  auto hit = index_.find(tmp_item);
  if (hit == index_.end()) {
    if (size() == capacity_) {
      auto* back_item = cache_.pop_back();
      auto  it        = index_.find(*back_item);
      index_.erase(it);
    }

    hit = index_.insert(std::move(tmp_item)).first;
    cache_.push_front(const_cast<item*>(&*hit));

    return;
  }

  // TODO Do update

  promote(hit);
}

template <typename Key, typename Value>
auto lru<Key, Value>::get(Key const& key) const -> Value* {
  auto tmp_item = item{std::move(key)};

  auto hit = index_.find(tmp_item);
  if (hit == index_.end()) {
    return nullptr;
  }

  promote(hit);

  return &const_cast<Value&>(hit->value);
}

template <typename Key, typename T>
auto lru<Key, T>::erase(Key const& key) -> void {
  auto tmp_item = item{std::move(key)};

  auto hit = index_.find(tmp_item);
  if (hit != index_.end()) {
    auto erase_item = const_cast<item*>(&*hit);
    erase_item->detach();
    index_.erase(hit);
  }
}

template <typename Key, typename T>
auto lru<Key, T>::size() const -> std::size_t {
  return index_.size();
}

template <typename Key, typename T>
auto lru<Key, T>::promote(const index::iterator& it) const -> void {
  auto tmp_item = const_cast<item*>(&*it);
  tmp_item->detach();
  cache_.push_front(tmp_item);
}

}  // namespace dsac
