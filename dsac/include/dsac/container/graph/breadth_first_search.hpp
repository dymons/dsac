#pragma once

#include <dsac/container/graph/directed_graph.hpp>

#include <optional>

namespace dsac {

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
std::optional<std::vector<typename directed_graph<N, E, A, B>::node_iterator>> bfs(
    directed_graph<N, E, A, B> const&                         graph,
    typename directed_graph<N, E, A, B>::node_key_type const& source,
    typename directed_graph<N, E, A, B>::node_key_type const& destination);

}  // namespace dsac

#define CONTAINER_BREADTH_FIRST_SEARCH_HPP
#include <dsac/container/graph/detail/breadth_first_search-inl.hpp>
#undef CONTAINER_BREADTH_FIRST_SEARCH_HPP