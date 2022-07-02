#pragma once

#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>
#include <dsac/container/graph/directed_graph.hpp>

#include <fmt/format.h>

namespace dsac {

template <typename N, typename E, typename A = std::allocator<N>, typename B = std::allocator<E>>
[[nodiscard]] directed_graph<N, E, A, B> read_graphviz([[maybe_unused]] std::string_view graphviz) {
  graphviz_syntax_analyzer syntax(graphviz);
  ast_base_ref statements = visit([&](ast_direct_graph_node const& g) { return g.get_statements(); }, syntax.parse());

  directed_graph<N, E, A, B> graph;

  std::function<ast_base_ref(ast_direct_edge_node const&)> process_statement;
  process_statement = [&](ast_direct_edge_node const& edge) {
    auto* from_cast = unsafe_to<ast_node_node>(edge.get_from_node());
    auto* to_cast   = unsafe_to<ast_node_node>(edge.get_to_node());
    if (to_cast == nullptr) {
      to_cast = unsafe_to<ast_node_node>(visit(process_statement, edge.get_to_node()));
    }

    const std::string from_name = static_cast<std::string>(from_cast->get_name());
    const std::string to_name   = static_cast<std::string>(to_cast->get_name());

    auto from = graph.insert_node(N::make(from_name));
    auto to   = graph.insert_node(N::make(to_name));

    const std::string edge_key = fmt::format("{}_{}", from_name, to_name);
    graph.insert_direct_edge(from.first, to.first, E::make(edge_key));

    return edge.get_from_node();
  };

  visit(
      [&process_statement](ast_statements_node const& statements) {
        for (ast_base_ref const& statement : statements) {
          visit(process_statement, statement);
        }
        return nullptr;
      },
      statements);

  return graph;
}

}  // namespace dsac
