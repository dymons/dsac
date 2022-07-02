#pragma once

#include <dsac/container/graph/directed_graph.hpp>

namespace dsac {

template <typename... Args>
std::vector<typename directed_graph<Args...>::node_iterator> bfs(
    directed_graph<Args...> const&                         graph,
    typename directed_graph<Args...>::node_key_type const& source,
    typename directed_graph<Args...>::node_key_type const& destination);

}  // namespace dsac

#define ALGORITHM_BREADTH_FIRST_SEARCH_HPP
#include <dsac/algorithm/detail/breadth_first_search-inl.hpp>
#undef ALGORITHM_BREADTH_FIRST_SEARCH_HPP