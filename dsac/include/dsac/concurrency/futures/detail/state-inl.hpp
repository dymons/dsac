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
executor_base_ref shared_state<T>::get_executor() {
  return executor_.is_empty() ? nullptr : executor_.read_only();
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

}  // namespace dsac
