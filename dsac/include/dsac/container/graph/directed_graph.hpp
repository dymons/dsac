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

class directed_graph_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class loops_not_supported : public directed_graph_exception {
public:
  loops_not_supported()
    : directed_graph_exception("at the moment loops are not supported in the directed graph") {
  }
};

template <typename T>
struct directed_graph_semantic {
  inline static auto get_key([[maybe_unused]] T const& entity) {
    return nullptr;
  }
};

template <typename Container>
struct proxy_node {
  using node_type    = typename Container::node_type;
  using edge_pointer = typename Container::edge_pointer;

  explicit proxy_node(node_type const& node)
    : node(node) {
  }

  node_type                         node;
  dsac::dynamic_array<edge_pointer> edges;
};
template <typename Container>
struct proxy_edge {
  using edge_type    = typename Container::edge_type;
  using node_pointer = typename Container::node_pointer;

  explicit proxy_edge(edge_type const& edge)
    : edge(edge) {
  }

  edge_type    edge;
  node_pointer from;
  node_pointer to;
};

template <typename Container>
class normal_node_iterator {
public:
  using pointer   = typename Container::node_pointer;
  using reference = typename Container::node_reference;

  normal_node_iterator()
    : current_(pointer()) {
  }

  normal_node_iterator(const pointer& iterator)  // NOLINT(google-explicit-constructor)
    : current_(iterator) {
  }

  reference operator*() const {
    return current_->node;
  }

  const pointer& base() const {
    return current_;
  }

private:
  pointer current_;
};
template <typename Container>
inline bool operator==(const normal_node_iterator<Container>& lhs, const normal_node_iterator<Container>& rha) {
  return lhs.base() == rha.base();
}
template <typename Container>
class normal_edge_iterator {
public:
  using pointer   = typename Container::edge_pointer;
  using reference = typename Container::edge_reference;

  normal_edge_iterator()
    : current_(pointer()) {
  }

  normal_edge_iterator(const pointer& iterator)  // NOLINT(google-explicit-constructor)
    : current_(iterator) {
  }

  reference operator*() const {
    return current_->edge;
  }

  const pointer& base() const {
    return current_;
  }

private:
  pointer current_;
};
template <typename Container>
inline bool operator==(const normal_edge_iterator<Container>& lhs, const normal_edge_iterator<Container>& rha) {
  return lhs.base() == rha.base();
}
template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
class directed_graph final {
public:
  using node_type             = N;
  using edge_type             = E;
  using node_allocator_type   = typename std::allocator_traits<A>::template rebind_alloc<proxy_node<directed_graph>>;
  using node_allocator_traits = typename std::allocator_traits<node_allocator_type>;
  using node_size_type        = typename node_allocator_traits::size_type;
  using node_difference_type  = typename node_allocator_traits::difference_type;
  using node_reference        = node_type&;
  using node_const_reference  = node_type const&;
  using node_pointer          = typename node_allocator_traits::pointer;
  using node_const_pointer    = typename node_allocator_traits::const_pointer;
  using node_iterator         = normal_node_iterator<directed_graph>;
  using edge_allocator_type   = typename std::allocator_traits<B>::template rebind_alloc<proxy_edge<directed_graph>>;
  using edge_allocator_traits = typename std::allocator_traits<edge_allocator_type>;
  using edge_size_type        = typename edge_allocator_traits::size_type;
  using edge_difference_type  = typename edge_allocator_traits::difference_type;
  using edge_reference        = edge_type&;
  using edge_const_reference  = edge_type const&;
  using edge_pointer          = typename edge_allocator_traits::pointer;
  using edge_const_pointer    = typename edge_allocator_traits::const_pointer;
  using edge_iterator         = normal_edge_iterator<directed_graph>;

private:
  using node_key_type = typename directed_graph_semantic<node_type>::key_type;
  using edge_key_type = typename directed_graph_semantic<edge_type>::key_type;

public:
  ~directed_graph() {
    for (auto& [_, node] : nodes_) {
      node_allocator_traits::destroy(node_allocator_, node);
      node_allocator_traits::deallocate(node_allocator_, node, 1U);
    }
    for (auto& [_, edge] : edges_) {
      edge_allocator_traits::destroy(edge_allocator_, edge);
      edge_allocator_traits::deallocate(edge_allocator_, edge, 1U);
    }
  }

  auto insert_node(node_type const& node) -> std::pair<node_iterator, bool> {
    node_key_type const key = directed_graph_semantic<node_type>::get_key(node);
    if (nodes_.contains(key)) {
      return std::make_pair(nodes_[key], false);
    }

    node_pointer pointer     = make_proxy_entity_for(node, node_allocator_);
    auto const [it, success] = nodes_.emplace(key, pointer);
    return std::make_pair(it->second, success);
  }

  auto insert_edge(node_iterator from, node_iterator to, edge_type const& edge) -> std::pair<edge_iterator, bool> {
    if (from == to) {
      throw loops_not_supported{};
    }

    edge_key_type const key  = directed_graph_semantic<edge_type>::get_key(edge);
    if (edges_.contains(key)) {
      return std::make_pair(edges_[key], false);
    }

    edge_pointer pointer = make_proxy_entity_for(edge, edge_allocator_);
    pointer->from        = from.base();
    pointer->to          = to.base();
    pointer->from->edges.push_back(pointer);
    pointer->to->edges.push_back(pointer);


    auto const [it, success] = edges_.emplace(key, pointer);
    return std::make_pair(it->second, success);
  }

private:
  template <typename Entity, typename Allocator>
  static auto make_proxy_entity_for(Entity const& entity, Allocator& allocator) {
    auto pointer = dsac::allocate(1U, allocator);
    try {
      dsac::construct(pointer, allocator, entity);
    } catch (...) {
      dsac::deallocate(pointer, 1U, allocator);
      throw;
    }

    return pointer;
  }

  std::unordered_map<node_key_type, node_pointer> nodes_;
  node_allocator_type                             node_allocator_;
  std::unordered_map<edge_key_type, edge_pointer> edges_;
  edge_allocator_type                             edge_allocator_;
};

}  // namespace dsac
