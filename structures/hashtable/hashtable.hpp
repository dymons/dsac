#pragma once

#include <vector>
#include <stdexcept>

namespace algo::hashtable {
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashTable final {
  class Node final {
    Key key_;
    Value value_;
    Node* bucket_;

   public:
    explicit Node(Key key, Value value) : key_(key), value_(value), bucket_(nullptr) {
    }

    void SetKey(Key key) {
      key_ = key;
    }

    void SetValue(Value value) {
      value_ = value;
    }

    void SetBucket(Node* bucket) {
      bucket_ = bucket;
    }

    Key GetKey() const noexcept {
      return key_;
    }

    Value GetValue() const noexcept {
      return value_;
    }

    Node* GetBucket() const noexcept {
      return bucket_;
    }

    void Destroy() {
      if (bucket_ != nullptr) {
        bucket_->Destroy();
        delete bucket_;
      }
    }
  };

  [[nodiscard]] std::size_t Hashing(Key key) const {
    return Hash{}(key) % hashtable_.size();
  }

  void DestroyImpl(std::vector<Node*>& hashtable) {
    for (Node*& node : hashtable) {
      if (node != nullptr) {
        node->Destroy();
        delete node;
      }
    }
  }

  void InsertImpl(Key key, Value value) {
    std::size_t const hash = Hashing(key);

    Node* parent = nullptr;
    Node* child = hashtable_[hash];

    while (child != nullptr && child->GetKey() != key) {
      parent = child;
      child = child->GetBucket();
    }

    if (child == nullptr) {
      child = new Node{key, value};
      if (parent == nullptr) {
        hashtable_[hash] = child;
      } else {
        parent->SetBucket(child);
      }
      ++buffer_size_;
    } else {
      child->SetValue(value);
    }
  }

  void Rehash() {
    std::vector<Node*> latest_hashtable(hashtable_.size() * 2, nullptr);
    std::swap(latest_hashtable, hashtable_);

    for (Node* node : latest_hashtable) {
      Node* entry = node;
      while (entry != nullptr) {
        InsertImpl(entry->GetKey(), entry->GetValue());
        entry = entry->GetBucket();
      }
    }

    DestroyImpl(latest_hashtable);
  }

  void TryRehash() {
    double const alpha = static_cast<double>(buffer_size_) / hashtable_.size();
    if (alpha > rehash_size_) {
      Rehash();
    }
  }

  constexpr static const int default_size_ = 8;
  constexpr static const double rehash_size_ = 0.75;
  int buffer_size_ = 0;

  std::vector<Node*> hashtable_;

 public:
  HashTable() : hashtable_(default_size_, nullptr) {
  }

  ~HashTable() {
    DestroyImpl(hashtable_);
  }

  void Insert(Key key, Value value) {
    TryRehash();
    InsertImpl(key, value);
  }

  [[nodiscard]] bool Contains(Key key) const {
    std::size_t const hash = Hashing(key);

    Node* entry = hashtable_[hash];
    while (entry != nullptr) {
      if (entry->GetKey() == key) {
        return true;
      }
      entry = entry->GetBucket();
    }

    return false;
  }

  [[nodiscard]] Value GetValue(Key key) const {
    std::size_t const hash = Hashing(key);

    Node* entry = hashtable_[hash];
    while (entry != nullptr) {
      if (entry->GetKey() == key) {
        return entry->GetValue();
      }
      entry = entry->GetBucket();
    }

    throw std::out_of_range{""};
  }

  void Remove(Key key) {
    std::size_t const hash = Hashing(key);

    Node* parent = nullptr;
    Node* child = hashtable_[hash];

    while (child != nullptr && child->GetKey() != key) {
      parent = child;
      child = child->GetBucket();
    }

    if (child != nullptr) {
      if (parent == nullptr) {
        hashtable_[hash] = child->GetBucket();
      } else {
        parent->SetBucket(child->GetBucket());
      }

      --buffer_size_;
      delete child;
    }
  }
};
}  // namespace algo::hashtable