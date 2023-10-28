#ifndef CONCURRENCY_LRU_CACHE_HPP
#error This file may only be included from lru.hpp
#endif

namespace dsac {

template <typename Key, typename T>
lru<Key, T>::lru(const std::size_t capacity)
  : capacity_(capacity) {
}

template <typename Key, typename T>
auto lru<Key, T>::put(Key key, T value) -> bool {
  auto hit = hash_.find(key);
  if (hit == hash_.end()) {
    if (size() == capacity_) {
      /*
       * TODO(dymons) >
       *    Need to find latest element with unpinned status.
       *      If element does not exist then return false.
       *      If element exist then remove them.
       */
      hash_.erase(cache_.back());
      cache_.pop_back();
    }

    cache_.push_front(std::move(value));
    hash_.emplace(std::move(key), cache_.begin());

    return true;
  }

  if (hit->second != cache_.begin()) {
    cache_.splice(cache_.begin(), cache_, hit->second, std::next(hit->second));
  }

  return false;
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
auto lru<Key, T>::pin([[maybe_unused]] Key const& key) -> void {
  /*
   *  TODO(dymons) >
   *      Need to pin/unpin elements at the cache_. But how?
   *      Maybe create a new container as dsac::marked_list,
   *      for the first implementation will be a decorator
   *      and will have the following properties:
   *       - If element will pinned then it is not taken into account in size
   *       - If element will pinned then taken into account splices
   *       - If element will pinned then not seen in all public methods
   *
   *      \code
   *          auto cache = dsac::marked_list{1};
   *
   *          cache.mark(1);
   *          REQUIRED(cache.empty());
   *
   *          cache.unmark(1);
   *          REQUIRED(not cache.empty());
   *      \endcode
   * */
}

template <typename Key, typename T>
auto lru<Key, T>::unpin([[maybe_unused]] Key const& key) -> void {
}

template <typename Key, typename T>
auto lru<Key, T>::size() const -> std::size_t {
  return cache_.size();
}

}  // namespace dsac
