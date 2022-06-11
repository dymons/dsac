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

template <typename T>
struct directed_graph_semantic {
  inline static auto get_key([[maybe_unused]] T const& entity) {
    return nullptr;
  }
};

template <typename N, typename E>
struct proxy_node {
  using node_type = N;
  using edge_type = E;

  explicit proxy_node(node_type const& node)
    : node(node) {
  }

  node_type node;
};
template <typename N, typename E>
struct proxy_edge {};
template <typename Iterator, typename DirectedGraph>
class normal_node_iterator {
  Iterator current_;

public:
  using reference = typename DirectedGraph::node_reference;

  normal_node_iterator()
    : current_(Iterator()) {
  }

  normal_node_iterator(const Iterator& iterator)  // NOLINT(google-explicit-constructor)
    : current_(iterator) {
  }

  reference operator*() const {
    return current_->node;
  }

  const Iterator& base() const {
    return current_;
  }
};
template <typename IteratorL, typename IteratorR, typename DirectedGraph>
inline bool operator==(
    const normal_node_iterator<IteratorL, DirectedGraph>& lhs,
    const normal_node_iterator<IteratorR, DirectedGraph>& rha) {
  return lhs.base() == rha.base();
}
template <typename Iterator>
class normal_edge_iterator {};
template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
class directed_graph final {
public:
  using node_type             = N;
  using edge_type             = E;
  using node_allocator_type   = typename std::allocator_traits<A>::template rebind_alloc<proxy_node<N, E>>;
  using node_allocator_traits = typename std::allocator_traits<node_allocator_type>;
  using node_size_type        = typename node_allocator_traits::size_type;
  using node_difference_type  = typename node_allocator_traits::difference_type;
  using node_reference        = node_type&;
  using node_const_reference  = node_type const&;
  using node_pointer          = typename node_allocator_traits::pointer;
  using node_const_pointer    = typename node_allocator_traits::const_pointer;
  using node_iterator         = normal_node_iterator<node_pointer, directed_graph>;
  using edge_allocator_type   = typename std::allocator_traits<B>::template rebind_alloc<proxy_edge<N, E>>;
  using edge_allocator_traits = typename std::allocator_traits<edge_allocator_type>;
  using edge_size_type        = typename edge_allocator_traits::size_type;
  using edge_difference_type  = typename edge_allocator_traits::difference_type;
  using edge_reference        = proxy_edge<N, E>&;
  using edge_const_reference  = proxy_edge<N, E> const&;
  using edge_pointer          = typename edge_allocator_traits::pointer;
  using edge_const_pointer    = typename edge_allocator_traits::const_pointer;
  using edge_iterator         = normal_edge_iterator<edge_pointer>;

private:
  using node_key_type = typename directed_graph_semantic<node_type>::key_type;

public:
  auto insert_node(node_type const& node) -> std::pair<node_iterator, bool> {
    node_key_type const key = directed_graph_semantic<node_type>::get_key(node);
    if (nodes_.contains(key)) {
      return std::make_pair(nodes_[key], false);
    }

    node_pointer pointer                = make_proxy_node(node, node_allocator_);
    [[maybe_unused]] auto const [it, _] = nodes_.emplace(key, pointer);
    return std::make_pair(pointer, true);
  }

private:
  static auto make_proxy_node(node_type const& node, node_allocator_type& allocator) -> node_pointer {
    constexpr node_size_type kEntities = 1U;
    node_pointer             pointer   = dsac::allocate(kEntities, allocator);
    try {
      dsac::construct(pointer, allocator, node);
    } catch (...) {
      dsac::deallocate(pointer, kEntities, allocator);
      throw;
    }
    return pointer;
  }

  std::unordered_map<node_key_type, node_pointer> nodes_;
  node_allocator_type                             node_allocator_;
};

}  // namespace dsac
