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

ast_edge_node::ast_edge_node(std::string_view operator_, ast_base_ref left_child, ast_base_ref right_child)
  : operator_(operator_)
  , left_child_(std::move(left_child))
  , right_child_(std::move(right_child)) {
}

void ast_attributes_node::insert_attribute(ast_base_ref attribute) {
  attributes_.push_back(std::move(attribute));
}

ast_attribute_node::ast_attribute_node(std::string_view key, std::string_view value)
  : key_(key)
  , value_(value) {
}

}  // namespace dsac
