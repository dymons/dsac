#ifndef CONCURRENCY_STATE_HPP
#error This file may only be included from state.hpp
#endif

namespace dsac {

template <typename T>
shared_state<T>::shared_state()
  : state_(detail::state::start) {
}

template <typename T>
bool shared_state<T>::has_result() const {
  detail::state const current_state = state_.load(std::memory_order_acquire);
  return detail::state() != (current_state & (detail::state::only_result | detail::state::done));
}

template <typename T>
bool shared_state<T>::has_callback() const {
  detail::state const current_state = state_.load(std::memory_order_acquire);
  return detail::state() != (current_state & (detail::state::only_callback | detail::state::done));
}

template <typename T>
result<T> shared_state<T>::get_result() {
  while (!has_result()) {
  }
  return *result_;
}

template <typename T>
executor_base_ref shared_state<T>::get_executor() const {
  return executor_;
}

template <typename T>
void shared_state<T>::set_result(result<T>&& result) {
  assert(!has_result());

  result_.emplace(std::move(result));

  detail::state current_state = state_.load(std::memory_order_acquire);
  switch (current_state) {
    case detail::state::start: {
      if (std::atomic_compare_exchange_strong_explicit(
              &state_, &current_state, detail::state::only_result, std::memory_order_release, std::memory_order_acquire
          )) {
        return;
      }
      assert(current_state == detail::state::only_callback);
      [[fallthrough]];
    }
    case detail::state::only_callback: {
      state_.store(detail::state::done, std::memory_order_relaxed);
      do_callback();
      return;
      default:
        throw shared_state_unexpected_state{};
    }
  }
}

template <typename T>
void shared_state<T>::set_executor(executor_base_ref exec) {
  executor_ = std::move(exec);
}

template <typename T>
void shared_state<T>::set_callback(callback<T>&& callback) {
  assert(!has_callback());

  callback_.emplace(std::move(callback));

  detail::state current_state = state_.load(std::memory_order_acquire);
  if (current_state == detail::state::start) {
    if (std::atomic_compare_exchange_strong_explicit(
            &state_, &current_state, detail::state::only_callback, std::memory_order_release, std::memory_order_acquire
        )) {
      return;
    }
    assert(current_state == detail::state::only_result);
  }

  if (current_state == detail::state::only_result) {
    state_.store(detail::state::done, std::memory_order_relaxed);
    do_callback();
    return;
  }

  throw shared_state_unexpected_state{};
}

template <typename T>
void shared_state<T>::do_callback() {
  assert(state_ == detail::state::done);

  auto data = *result_;
  if (executor_base_ref executor = get_executor(); executor != nullptr) {
    executor->submit([callback = std::move(callback_), data = std::move(data)]() mutable {
      (*callback)(std::move(data));
    });
  } else {
    (*callback_)(std::move(data));
  }
}

template <typename T>
hold_state<T>::hold_state(state_ref<T> state)
  : state_(std::move(state)) {
}

template <typename T>
state_ref<T> hold_state<T>::release_state() && {
  assert(static_cast<bool>(state_));
  return std::move(state_);
}

}  // namespace dsac
