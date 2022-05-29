#ifndef CONCURRENCY_STATE_HPP
#error This file may only be included from state.hpp
#endif

namespace dsac {

template <typename T>
shared_state<T>::shared_state()
  : storage_(dsac::make_shared<mvar<state_value>>()) {
}

template <typename T>
bool shared_state<T>::has_result() const {
  const auto result = storage_->template try_with_lock([](state_value const& state) { return state.index() == 0; });
  return result.has_value() && result.value();
}

template <typename T>
bool shared_state<T>::has_callback() const {
  const auto result = storage_->template try_with_lock([](state_value const& state) { return state.index() == 1; });
  return result.has_value() && result.value();
}

template <typename T>
result<T> shared_state<T>::get_result() {
  // We are waiting for the user value to be saved in the storage_
  has_value_.read_only();

  // Then we try to get this value from the storage_
  return std::get<result<T>>(storage_->read_only());
}

template <typename T>
executor_base_ref shared_state<T>::get_executor() const {
  return executor_;
}

template <typename T>
void shared_state<T>::set_result(result<T>&& result) {
  throw_if_fulfilled_by_result();

  if (has_callback()) {
    callback<T> handler = std::get<callback<T>>(storage_->take());
    storage_->put(std::move(result));
    has_value_.put();
    do_callback(std::move(handler));
  } else {
    storage_->put(std::move(result));
    has_value_.put();
  }
}

template <typename T>
void shared_state<T>::set_executor(executor_base_ref exec) {
  executor_ = std::move(exec);
}

template <typename T>
void shared_state<T>::set_callback(callback<T>&& callback) {
  throw_if_fulfilled_by_callback();

  if (has_result()) {
    do_callback(std::move(callback));
  } else {
    storage_->put(std::move(callback));
  }
}

template <typename T>
void shared_state<T>::do_callback(callback<T>&& callback) {
  storage_->template try_with_lock([this, &callback](state_value const& state) -> bool {
    auto data = std::get<result<T>>(state);
    if (executor_base_ref executor = get_executor(); executor != nullptr) {
      executor->submit(
          [callback = std::move(callback), data = std::move(data)]() mutable { callback(std::move(data)); });
    } else {
      callback(std::move(data));
    }
    return true;
  });
}

template <typename T>
void shared_state<T>::throw_if_fulfilled_by_result() {
  if (has_result()) {
    throw shared_state_already_satisfied{};
  }
}

template <typename T>
void shared_state<T>::throw_if_fulfilled_by_callback() {
  if (has_callback()) {
    throw shared_state_already_satisfied{};
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
