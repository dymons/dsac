#pragma once

#include <dsac/container/graph/directed_graph.hpp>

namespace dsac {

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
[[nodiscard]] directed_graph<N, E, A, B> read_graphviz(std::string_view graphviz) {

  /*
   *            Simple context-free grammar for GraphViz
   *
   *  graph          : (graph | digraph) ID LPARAM statement_list RPARAM
   *  statement_list : statement SEMI | statement SEMI statement_list
   *  statement      : factor ((--,->)factor)*
   *  factor         : node
   *
   * */

  directed_graph<N, E, A, B> graph;

  return graph;
}

}  // namespace dsac
