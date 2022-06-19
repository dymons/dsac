#pragma once

#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>
#include <dsac/container/graph/directed_graph.hpp>

namespace dsac {

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
[[nodiscard]] directed_graph<N, E, A, B> read_graphviz([[maybe_unused]] std::string_view graphviz) {
  return {};
}

}  // namespace dsac
