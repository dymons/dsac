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

  private:
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

  auto insert(Key key, Value value) -> void {
  }
};

}  // namespace dsac
