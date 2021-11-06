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
  };

  [[nodiscard]] std::size_t Hashing(Key key) const {
    return Hash{}(key) % hashtable_.size();
  }

  static const int default_size_ = 8;
  std::vector<Node*> hashtable_;

 public:
  HashTable() : hashtable_(default_size_, nullptr) {
  }

  void Insert(Key key, Value value) {
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
    } else {
      child->SetValue(value);
    }
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
      delete child;
    }
  }
};
}  // namespace algo::hashtable