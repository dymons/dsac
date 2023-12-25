#pragma once

namespace dsac::intrusive {

template <typename T>
struct list_node_base {
  // Observers

  /*!
    \brief
        Returns the pointer to the previous node
  */
  [[nodiscard]] auto get_prev() const noexcept -> list_node_base*;

  /*!
    \brief
        Returns the pointer to the next node
  */
  [[nodiscard]] auto get_next() const noexcept -> list_node_base*;

  // Modifiers

  /*!
    \brief
        Set the pointer to the next node
  */
  auto set_next(list_node_base* next) noexcept -> void;

  /*!
    \brief
        Set the pointer to the prev node
  */
  auto set_prev(list_node_base* prev) noexcept -> void;

  /*!
    \brief
        Attach a node after the current one

    \param next
        A node to link
  */
  auto attach(list_node_base* next) noexcept -> void;

  /*!
    \brief
        Detach a node from the list
  */
  auto detach() noexcept -> void;

private:
  /// A the pointer to the previous node
  list_node_base* prev_{nullptr};

  /// A the pointer to the next node
  list_node_base* next_{nullptr};
};

template <typename T>
class list final {
public:
  // Constructors

  /*!
    \brief
        Default constructor, constructs an empty list.
  */
  list() noexcept;

  // Observers

  /*!
    \brief
        Returns true if the list is empty.
  */
  [[nodiscard]] auto empty() const noexcept -> bool;

  // Modifiers

  auto push_back(list_node_base<T>* next) -> void;

  auto push_front(list_node_base<T>* next) -> void;

  auto pop_front() noexcept -> T*;

  auto pop_back() noexcept -> T*;

private:
  list_node_base<T> storage_{};
};

}  // namespace dsac::intrusive

#define CONTAINER_INTRUSIVE_LIST_HPP
#include <dsac/container/intrusive/detail/list-inl.hpp>
#undef CONTAINER_INTRUSIVE_LIST_HPP
