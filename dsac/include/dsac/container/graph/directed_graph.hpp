#pragma once

#include <dsac/container/dynamic_array.hpp>
#include <dsac/memory/shared_ptr.hpp>

#include <functional>
#include <unordered_map>
#include <vector>

namespace dsac::legacy_graph {
struct Node final {
  int                         id{};
  [[gnu::always_inline]] bool operator==(const Node other) const noexcept {
    return id == other.id;
  }
};
}  // namespace dsac::legacy_graph

namespace std {
template <>
struct hash<dsac::legacy_graph::Node> {
  [[gnu::always_inline]] std::size_t operator()(dsac::legacy_graph::Node node) const noexcept {
    return node.id;
  }
};
}  // namespace std

namespace dsac::legacy_graph {
class Digraph final {
public:
  using Successors    = std::vector<Node>;
  using AdjacencyList = std::unordered_map<Node, Successors>;
  using Visitor       = std::function<void(Node)>;

  void AddEdge(Node from, Node to) {
    graph_[from].push_back(to);
  }

  Successors const& GetSuccessors(const Node node) {
    return graph_[node];
  }

  void Visit(Visitor visitor) const {
    for (const auto& [node, successors] : graph_) {
      visitor(node);
    }
  }

private:
  AdjacencyList graph_;
};
}  // namespace dsac::legacy_graph

namespace dsac {

namespace detail {

template <typename T>
struct nth_key {
  inline static auto get([[maybe_unused]] T const& entity) {
    return nullptr;
  }
};

}  // namespace detail

template <typename Node, typename Edge>
class proxy_node {
  using node_type = Node;
  using edge_type = Edge;
};
template <typename Node, typename Edge>
class proxy_edge {
  using node_type = Node;
  using edge_type = Edge;
};
template <typename Node, typename Edge>
class normal_edge_iterator {
  using node_type = Node;
  using edge_type = Edge;
};
template <typename Node, typename Edge>
class normal_node_iterator {
  using node_type = Node;
  using edge_type = Edge;
};

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
class directed_graph final {
public:
  using node_type             = N;
  using edge_type             = E;
  using node_allocator_type   = A;
  using node_allocator_traits = typename std::allocator_traits<node_allocator_type>;
  using node_size_type        = typename node_allocator_traits::size_type;
  using node_difference_type  = typename node_allocator_traits::difference_type;
  using node_reference        = proxy_node<node_type, edge_type>&;
  using node_const_reference  = proxy_node<node_type, edge_type> const&;
  using node_pointer          = typename node_allocator_traits::pointer;
  using node_const_pointer    = typename node_allocator_traits::const_pointer;
  using edge_allocator_type   = B;
  using edge_allocator_traits = typename std::allocator_traits<edge_allocator_type>;
  using edge_size_type        = typename edge_allocator_traits::size_type;
  using edge_difference_type  = typename edge_allocator_traits::difference_type;
  using edge_reference        = proxy_edge<node_type, edge_type>&;
  using edge_const_reference  = proxy_edge<node_type, edge_type> const&;
  using edge_pointer          = typename edge_allocator_traits::pointer;
  using edge_const_pointer    = typename edge_allocator_traits::const_pointer;
  using node_iterator         = normal_node_iterator<node_type, edge_type>;
  using edge_iterator         = normal_edge_iterator<node_type, edge_type>;
};

}  // namespace dsac
