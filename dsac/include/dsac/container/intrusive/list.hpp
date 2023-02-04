#pragma once

namespace dsac::intrusive {

template <typename T>
struct list_node_base {
  // Observers

  /*!
    \brief
        Returns the pointer to the previous node
  */
  [[nodiscard]] auto get_prev() noexcept -> list_node_base*;

  /*!
    \brief
        Returns the pointer to the next node
  */
  [[nodiscard]] auto get_next() noexcept -> list_node_base*;

  // Modifiers

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
private:
  list_node_base<T> root_;
};

}  // namespace dsac::intrusive

#define CONTAINER_INTRUSIVE_LIST_HPP
#include <dsac/container/intrusive/detail/list-inl.hpp>
#undef CONTAINER_INTRUSIVE_LIST_HPP
