#pragma once

#include <concurrency/support/noncopyable.hpp>
#include <concurrency/support/nonmovable.hpp>

#include <optional>
#include <mutex>
#include <condition_variable>

namespace algo::syncing {
template <typename T>
class MVar : public detail::NonCopyable, public detail::NonMovable {
  std::optional<T> storage_;

  std::mutex mutex_;
  std::condition_variable has_value_storage_;
  std::condition_variable not_value_storage_;

 public:
  MVar() = default;
  explicit MVar(T value) : storage_(std::move(value)) {
  }

  void Put(T data) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_value_storage_.wait(lock, [this] {
        return !storage_.has_value();
      });

      storage_.emplace(std::move(data));
    }

    has_value_storage_.notify_all();
  }

  T Take() {
    std::optional<T> result;

    {
      std::unique_lock<std::mutex> lock(mutex_);
      has_value_storage_.wait(lock, [this] {
        return storage_.has_value();
      });
      result.swap(storage_);
    }

    not_value_storage_.notify_all();
    return *result;
  }

  [[nodiscard]] bool IsEmpty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return !storage_.has_value();
  }

  T const& ReadOnly() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] {
      return storage_.has_value();
    });
    return storage_.value();
  }
};

// TODO: Добавить специализацию класса MVar<void>
}  // namespace algo::syncing