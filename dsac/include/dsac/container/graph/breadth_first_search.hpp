#pragma once

namespace dsac {

// clang-format off
template <class Graph>
std::vector<typename Graph::node_iterator> bfs(
    Graph const&                         graph,
    typename Graph::node_key_type const& source,
    typename Graph::node_key_type const& destination);
// clang-format on

}  // namespace dsac

#define CONTAINER_BREADTH_FIRST_SEARCH_HPP
#include <dsac/container/graph/detail/breadth_first_search-inl.hpp>
#undef CONTAINER_BREADTH_FIRST_SEARCH_HPP