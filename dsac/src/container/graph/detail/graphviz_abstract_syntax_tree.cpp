#include <dsac/container/graph/detail/graphviz_abstract_syntax_tree.hpp>
#include <utility>

namespace dsac {

ast_graph_node::ast_graph_node(std::string_view graph_type, std::string_view graph_name, ast_base_ref child)
  : graph_type_(graph_type)
  , graph_name_(graph_name)
  , child_(std::move(child)) {
}

}  // namespace dsac
