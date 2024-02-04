#pragma once

#include <dsac/common/macros.h>
#include <dsac/container/dynamic_array.hpp>
#include <dsac/memory/shared_ptr.hpp>

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
    static auto make(std::size_t local_depth, std::size_t bucket_size) {
      return make_shared<extendible_hashtable_bucket>(new extendible_hashtable_bucket{local_depth, bucket_size});
    }

    auto insert(Key const& key, Value const& value) -> void {
      DSAC_ASSERT(has_capacity(), "Not enough space at extendible_hashtable_bucket");
      chain_.insert_or_assign(std::move(key), std::move(value));
    }

    auto local_depth() const -> std::size_t {
      return local_depth_;
    }

    auto mutable_local_depth() -> std::size_t& {
      return local_depth_;
    }

    auto has_capacity() const -> bool {
      return chain_.size() < bucket_size_;
    }

    auto capacity() const -> std::size_t {
      return bucket_size_;
    }

    auto begin() {
      return chain_.begin();
    }

    auto end() {
      return chain_.end();
    }

    auto contains(Key const& key) const -> bool {
      return chain_.contains(key);
    }

  private:
    explicit extendible_hashtable_bucket(std::size_t local_depth, std::size_t bucket_size)
      : local_depth_(local_depth)
      , bucket_size_(bucket_size) {
    }

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
    using directory = extendible_hashtable_directory;
    using bucket    = extendible_hashtable_bucket;

  public:
    explicit extendible_hashtable_directory(std::size_t global_depth, std::size_t bucket_size)
      : global_depth_(global_depth) {
      buckets_.reserve(std::size_t{1} << global_depth_);
      for (auto begin = std::size_t{}, end = std::size_t{1} << global_depth_; begin < end; ++begin) {
        buckets_.push_back(bucket::make(global_depth, bucket_size));
      }
    }

    auto insert(Key const& key, Value const& value) -> void {
      if (auto [original_bucket, index] = get_bucket_with_index_by_key(key); not original_bucket->has_capacity()) {
        DSAC_ASSERT(
            original_bucket->local_depth() <= global_depth_,
            "A bucket cannot have a depth greater than the directory depth"
        );

        // Step 1. A bucket split is only possible then
        // the local_depth of the bucket is less than
        // the global_depth of the directory. So, the
        // first step then is to expand the directory.
        if (original_bucket->local_depth() == global_depth_) {
          // Then double directory
          for (auto begin = std::size_t{}, end = buckets_.size(); begin < end; ++begin) {
            buckets_.push_back(buckets_[begin]);
          }

          // After double directory, we have to increase global depth
          ++global_depth_;
        }
        DSAC_ASSERT(
            original_bucket->local_depth() < global_depth_,
            "A bucket split is only possible if local_depth < global_depth"
        );

        // Step 2. The local depth of the original bucket
        // and its split image are set to local_depth + 1
        ++original_bucket->mutable_local_depth();

        // Step 3. So, we have enough entries, we have to split
        // our buckets into separate buckets for storing key/value.
        auto& split_image = buckets_[index ^ (std::size_t{1} << (original_bucket->local_depth() - 1))];
        DSAC_ASSERT(original_bucket == split_image, "Original Bucket is not equal to Split Image");

        // Step 4. Create new buckets and reinsert values from original_bucket
        split_image     = bucket::make(original_bucket->local_depth(), original_bucket->capacity());
        buckets_[index] = bucket::make(original_bucket->local_depth(), original_bucket->capacity());

        // Keys in each bucket now agree on the d + 1 least significant bit
        DSAC_ASSERT(true, "Keys in each bucket now agree on the d + 1");

        // Step 5. After an unlucky split all keys might be placed in
        // the one of the two new buckets. Possible leading to a cascade of splits
        for (auto it = original_bucket->begin(); it != original_bucket->end(); ++it) {
          insert(it->first, it->second);
        }

        // Step 6. After the split we attempt to add k again.
        // Typically, after the split the new bucket has free space
        insert(key, value);

        return;
      }

      // Have enough spaces, store our key/value to the bucket
      get_bucket_by_key(key)->insert(key, value);
    }

    auto size() const -> std::size_t {
      return buckets_.size();
    }

    auto contains(Key const& key) const -> bool {
      return get_bucket_by_key(key)->contains(key);
    }

  private:
    [[nodiscard]] auto get_hash(Key const& key) const -> std::size_t {
      return std::hash<Key>{}(key);
    }

    [[nodiscard]] auto get_bucket_with_index_by_key(Key const& key) const {
      auto const index = get_hash(key) & ((1 << global_depth_) - 1);
      return std::make_pair(buckets_[index], index);
    }

    [[nodiscard]] auto get_bucket_by_key(Key const& key) const {
      return get_bucket_with_index_by_key(key).first;
    }

    ///
    std::size_t global_depth_;

    ///
    dynamic_array<shared_ptr<bucket>> buckets_;
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

  auto size() const -> std::size_t {
    return this->directory.size();
  };

  auto contains(Key const& key) const -> bool {
    return this->directory.contains(key);
  }
};

}  // namespace dsac
