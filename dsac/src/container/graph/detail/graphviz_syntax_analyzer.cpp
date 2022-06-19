#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

#include <unordered_set>

namespace dsac {

namespace {

const std::unordered_set<std::string_view> kGraphTypes{"graph", "digraph"};
const std::string_view                     kOpenCurlyBracket   = "{";
const std::string_view                     kCloseCurlyBracket  = "}";
const std::string_view                     kOpenSquareBracket  = "[";
const std::string_view                     kCloseSquareBracket = "]";

}  // namespace

graphviz_syntax_analyzer::graphviz_syntax_analyzer(std::string_view graphviz)
  : tokenizer_(graphviz)
  , current_token_(tokenizer_.get_next_token()) {
}

ast_base_ref graphviz_syntax_analyzer::statement_list() {
  return nullptr;
}

ast_base_ref graphviz_syntax_analyzer::graph() {
  const std::string_view graph_type = get_token<token::keyword>();
  if (!kGraphTypes.contains(graph_type)) {
    throw unexpected_syntax{};
  }
  const std::string_view graph_name = get_token<token::identifier>();
  if (get_token<token::punctuator>() != kOpenCurlyBracket) {
    throw unexpected_syntax{};
  }

  ast_base_ref graph_root = dsac::make_shared<ast_graph_node>(graph_type, graph_name, statement_list());
  if (get_token<token::punctuator>() != kCloseCurlyBracket) {
    throw unexpected_syntax{};
  }

  return graph_root;
}

ast_base_ref graphviz_syntax_analyzer::parse() {
  return graph();
}

}  // namespace dsac
