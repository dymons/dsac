#include <dsac/container/graph/detail/graphviz_abstract_syntax_tree.hpp>
#include <utility>

namespace dsac {

ast_base::ast_base(kind kind)
  : kind_(kind) {
}

kind ast_base::what() noexcept {
  return kind_;
}

ast_direct_graph_node::ast_direct_graph_node(std::string_view graph_name, ast_base_ref child)
  : ast_base(kind::direct_graph)
  , graph_name_(graph_name)
  , child_(std::move(child)) {
}

std::string_view ast_direct_graph_node::get_graph_name() const noexcept {
  return graph_name_;
}

ast_undirect_graph_node::ast_undirect_graph_node(std::string_view graph_name, ast_base_ref child)
  : ast_base(kind::undirect_graph)
  , graph_name_(graph_name)
  , child_(std::move(child)) {
}

void ast_statements_node::insert_statement(ast_base_ref statement) {
  statements_.push_back(std::move(statement));
}

ast_node_node::ast_node_node(kind kind, std::string_view node_name, ast_base_ref attributes)
  : ast_base(kind)
  , node_name_(node_name)
  , attributes_(std::move(attributes)) {
}

ast_direct_edge_node::ast_direct_edge_node(ast_base_ref left_child, ast_base_ref right_child)
  : ast_base(kind::edge_direct_connection)
  , left_child_(std::move(left_child))
  , right_child_(std::move(right_child)) {
}

ast_undirect_edge_node::ast_undirect_edge_node(ast_base_ref left_child, ast_base_ref right_child)
  : ast_base(kind::edge_undirect_connection)
  , left_child_(std::move(left_child))
  , right_child_(std::move(right_child)) {
}

void ast_attributes_node::insert_attribute(ast_base_ref attribute) {
  attributes_.push_back(std::move(attribute));
}

ast_attribute_node::ast_attribute_node(kind kind, std::string_view key, std::string_view value)
  : ast_base(kind)
  , key_(key)
  , value_(value) {
}

}  // namespace dsac
