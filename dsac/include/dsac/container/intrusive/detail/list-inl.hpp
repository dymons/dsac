#ifndef CONTAINER_INTRUSIVE_LIST_HPP
#error This file may only be included from list.hpp
#endif

#include <utility>

namespace dsac::intrusive {

template <typename T>
auto list_node_base<T>::get_prev() noexcept -> list_node_base* {
  return prev_;
}

template <typename T>
auto list_node_base<T>::get_next() noexcept -> list_node_base* {
  return next_;
}

template <typename T>
auto list_node_base<T>::attach(list_node_base* next) noexcept -> void {
  prev_        = std::exchange(next->prev_, this);
  prev_->next_ = this;
  next_        = next;
}

template <typename T>
auto list_node_base<T>::detach() noexcept -> void {
  if (nullptr != next_) {
    next_->prev_ = prev_;
  }
  if (nullptr != prev_) {
    prev_->next_ = next_;
  }
  next_ = prev_ = nullptr;
}

}  // namespace dsac::intrusive