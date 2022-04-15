#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>

namespace dsac::syncing {
template <typename T>
class MVar {
  std::optional<T> storage_;

  std::mutex              mutex_;
  std::condition_variable has_value_storage_;
  std::condition_variable not_value_storage_;

public:
  MVar() = default;
  explicit MVar(T value)
    : storage_(std::move(value)) {
  }
  MVar(const MVar&)                    = delete;
  MVar(MVar&&)                         = delete;
  auto operator=(const MVar&) -> MVar& = delete;
  auto operator=(MVar&&) -> MVar&      = delete;
  ~MVar()                              = default;

  void Put(T data) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_value_storage_.wait(lock, [this] { return !storage_.has_value(); });

      storage_.emplace(std::move(data));
    }

    has_value_storage_.notify_all();
  }

  T Take() {
    std::optional<T> result;

    {
      std::unique_lock<std::mutex> lock(mutex_);
      has_value_storage_.wait(lock, [this] { return storage_.has_value(); });
      result.swap(storage_);
    }

    not_value_storage_.notify_all();
    return std::move(*result);
  }

  [[nodiscard]] bool IsEmpty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return !storage_.has_value();
  }

  T const& ReadOnly() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return storage_.has_value(); });
    return storage_.value();
  }
};

template <>
class MVar<void> {
  bool contains_ = false;

  std::mutex              mutex_;
  std::condition_variable has_value_storage_;
  std::condition_variable not_value_storage_;

public:
  MVar()                               = default;
  MVar(const MVar&)                    = delete;
  MVar(MVar&&)                         = delete;
  auto operator=(const MVar&) -> MVar& = delete;
  auto operator=(MVar&&) -> MVar&      = delete;
  ~MVar()                              = default;

  void Put() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_value_storage_.wait(lock, [this] { return !contains_; });

      contains_ = true;
    }

    has_value_storage_.notify_all();
  }

  void Take() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      has_value_storage_.wait(lock, [this] { return contains_; });
      contains_ = false;
    }

    not_value_storage_.notify_all();
  }

  [[nodiscard]] bool IsEmpty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return !contains_;
  }

  void ReadOnly() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return contains_; });
  }
};
}  // namespace dsac::syncing