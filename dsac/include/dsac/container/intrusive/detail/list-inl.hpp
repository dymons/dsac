#ifndef CONTAINER_INTRUSIVE_LIST_HPP
#error This file may only be included from list.hpp
#endif

#include <utility>

namespace dsac::intrusive {

template <typename T>
auto list_node_base<T>::get_prev() const noexcept -> list_node_base* {
  return prev_;
}

template <typename T>
auto list_node_base<T>::get_next() const noexcept -> list_node_base* {
  return next_;
}

template <typename T>
auto list_node_base<T>::attach(list_node_base* next) noexcept -> void {
  prev_        = std::exchange(next->prev_, this);
  prev_->next_ = this;
  next_        = next;
}

template <typename T>
auto list_node_base<T>::set_next(list_node_base* next) noexcept -> void {
  next_ = next;
}

template <typename T>
auto list_node_base<T>::set_prev(list_node_base* prev) noexcept -> void {
  prev_ = prev;
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

template <typename T>
list<T>::list() noexcept {
  storage_.set_next(&storage_);
  storage_.set_prev(&storage_);
}

template <typename T>
auto list<T>::empty() const noexcept -> bool {
  return storage_.get_next() == &storage_;
}

template <typename T>
auto list<T>::push_back(list_node_base<T>* next) -> void {
  next->attach(&storage_);
}

template <typename T>
auto list<T>::pop_front() noexcept -> T* {
  if (empty()) {
    return nullptr;
  }

  auto* front = storage_.get_next();
  front->detach();

  return static_cast<T*>(front);
}

template <typename T>
auto list<T>::pop_back() noexcept -> T* {
  if (empty()) {
    return nullptr;
  }

  auto* back = storage_.get_prev();

  back->detach();

  return static_cast<T*>(back);
}

template <typename T>
auto list<T>::push_front(list_node_base<T>* next) -> void {
  next->attach(storage_.get_next());
}

}  // namespace dsac::intrusive