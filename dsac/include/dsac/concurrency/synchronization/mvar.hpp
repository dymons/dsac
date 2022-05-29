#pragma once

#include <dsac/memory/shared_ptr.hpp>

#include <condition_variable>
#include <mutex>
#include <optional>

namespace dsac {
template <typename T>
class mvar final {
  std::optional<T> storage_;

  std::mutex              mutex_;
  std::condition_variable has_value_storage_;
  std::condition_variable not_value_storage_;

public:
  mvar() = default;
  explicit mvar(T value)
    : storage_(std::move(value)) {
  }
  mvar(const mvar&)                    = delete;
  mvar(mvar&&)                         = delete;
  auto operator=(const mvar&) -> mvar& = delete;
  auto operator=(mvar&&) -> mvar&      = delete;
  ~mvar()                              = default;

  void put(T&& data) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_value_storage_.wait(lock, [this] { return !storage_.has_value(); });

    storage_.emplace(std::forward<T>(data));
    has_value_storage_.notify_all();
  }

  T take() {
    std::optional<T> result;

    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return storage_.has_value(); });
    result.swap(storage_);

    not_value_storage_.notify_all();
    return std::move(*result);
  }

  [[nodiscard]] bool is_empty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return !storage_.has_value();
  }

  T const& read_only() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return storage_.has_value(); });
    return storage_.value();
  }

  template <class Function>
  auto try_with_lock(Function&& function) -> std::optional<typename std::result_of<Function(T const&)>::type> {
    using return_type = typename std::result_of<Function(T const&)>::type;

    std::lock_guard<std::mutex> lock(mutex_);
    if (!storage_.has_value()) {
      return std::nullopt;
    }

    return std::optional<return_type>(function(storage_.value()));
  }
};

template <>
class mvar<void> {
  bool contains_ = false;

  std::mutex              mutex_;
  std::condition_variable has_value_storage_;
  std::condition_variable not_value_storage_;

public:
  mvar()                               = default;
  mvar(const mvar&)                    = delete;
  mvar(mvar&&)                         = delete;
  auto operator=(const mvar&) -> mvar& = delete;
  auto operator=(mvar&&) -> mvar&      = delete;
  ~mvar()                              = default;

  void put() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_value_storage_.wait(lock, [this] { return !contains_; });

    contains_ = true;
    has_value_storage_.notify_all();
  }

  void take() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return contains_; });
    contains_ = false;
    not_value_storage_.notify_all();
  }

  [[nodiscard]] bool is_empty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return !contains_;
  }

  void read_only() {
    std::unique_lock<std::mutex> lock(mutex_);
    has_value_storage_.wait(lock, [this] { return contains_; });
  }
};

template <typename T>
using mvar_ref = dsac::shared_ptr<mvar<T>>;

}  // namespace dsac
