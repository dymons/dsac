#include <dsac/container/graph/detail/graphviz_abstract_syntax_tree.hpp>
#include <utility>

namespace dsac {

ast_graph_node::ast_graph_node(std::string_view graph_type, std::string_view graph_name, ast_base_ref child)
  : graph_type_(graph_type)
  , graph_name_(graph_name)
  , child_(std::move(child)) {
}

void ast_statement_list_node::insert_statement(ast_base_ref statement) {
  statements_.push_back(std::move(statement));
}

ast_node_node::ast_node_node(std::string_view node_name, ast_base_ref attributes)
  : node_name_(node_name)
  , attributes_(std::move(attributes)) {
}

}  // namespace dsac
