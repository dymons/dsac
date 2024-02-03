#pragma once

#include <dsac/container/dynamic_array.hpp>

#include <cstddef>
#include <map>

namespace dsac {

/*
 *                              Dynamic Hashing
 *
 * The capacity of the table begins small to accommodate initial data but grows |
 * as new data is added (or deleted). Dynamic hashing handles collisions  using |
 * chaining, so use fixed length chains so that a full chain fits in  a  single |
 * page of memory.
 *
 * At out implementation extendible hashing uses a directory of buckets, where  |
 * directories always have size equal to a power of two.
 *
 * Requirements:
 * - A directory with global depth D has 2^D entries
 * - Every entry of the directory points to a bucket
 * - Each bucket is a page of memory storing a single chain of key-value pairs
 * - Each bucket has a local depth d <= D
 *
 * */

template <typename Key, typename Value, typename KeyComparator>
struct extendible_hashtable_base {
  /*!
    \brief
        It is a bucket implementation
  */
  class extendible_hashtable_bucket final {
  public:
    explicit extendible_hashtable_bucket(std::size_t local_depth, std::size_t bucket_size)
      : local_depth_(local_depth)
      , bucket_size_(bucket_size) {
    }

    auto insert(Key const& key, Value const& value) -> void {
      chain_.insert_or_assign(key, value);
    }

    auto size() const -> std::size_t {
      return chain_.size();
    }

    auto capacity() const -> std::size_t {
      return bucket_size_;
    }

    auto local_depth() const -> std::size_t {
      return local_depth_;
    }

    auto mutable_local_depth() -> std::size_t& {
      return local_depth_;
    }

  private:
    ///
    std::size_t local_depth_;

    ///
    std::size_t bucket_size_;

    ///
    std::map<Key, Value, KeyComparator> chain_;
  };

  /*!
    \brief
        It is a directory implementation
  */
  class extendible_hashtable_directory final {
  public:
    explicit extendible_hashtable_directory(std::size_t global_depth, std::size_t bucket_size)
      : global_depth_(global_depth)
      , buckets_(1 << global_depth_, extendible_hashtable_bucket{global_depth_, bucket_size}) {
    }

    auto insert(Key const& key, Value const& value) -> void {
      auto* bucket = get_bucket_by_key(key);
      if (bucket->size() == bucket->capacity()) {
        // A bucket split is only possible then the local_depth
        // of the bucket is less than the global_depth
        // of the directory. So, the first step then
        // is to expand the directory.
        if (bucket->local_depth() >= global_depth_) {
          // The first step is to increase the local depth of the bucket
          ++bucket->mutable_local_depth();

          // Then double directory
          for (auto begin = std::size_t{}, end = buckets_.size(); begin < end; ++begin) {
            buckets_.push_back(buckets_[begin]);
          }

          // After double directory, we have to increase global depth
          ++global_depth_;
        }

        // So, we have enough entries, we have to split out buckets into
        // separate buckets for storing key/value
      }

      // After split the directory, we can store our key/value
      get_bucket_by_key(key)->insert(key, value);
    }

  private:
    [[nodiscard]] auto get_bucket_by_key(Key const& key) const -> extendible_hashtable_bucket* {
      return buckets_[std::hash(key) & ((1 << global_depth_) - 1)];
    }

    ///
    std::size_t global_depth_;

    ///
    dynamic_array<extendible_hashtable_bucket> buckets_;
  };

  explicit extendible_hashtable_base(std::size_t global_depth, std::size_t bucket_size)
    : directory{global_depth, bucket_size} {
  }

  extendible_hashtable_directory directory;
};

template <typename Key, typename Value, typename KeyComparator = std::less<Key>>
class extendible_hashtable final : protected extendible_hashtable_base<Key, Value, KeyComparator> {
  using base = extendible_hashtable_base<Key, Value, KeyComparator>;

public:
  /*!
    \brief
        User constructor, constructs an extendible_hashtable with predefined global_depth.
  */
  explicit extendible_hashtable(std::size_t global_depth, std::size_t bucket_size)
    : base(global_depth, bucket_size) {
  }

  auto insert(Key const& key, Value const& value) -> void {
    this->directory.insert(key, value);
  }
};

}  // namespace dsac
