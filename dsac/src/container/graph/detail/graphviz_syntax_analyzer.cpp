#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

#include <unordered_set>

namespace dsac {

namespace {

const std::unordered_set<std::string_view> kGraphTypes{"graph", "digraph"};
const std::string_view                     kOpenCurlyBracket   = "{";
const std::string_view                     kCloseCurlyBracket  = "}";
const std::string_view                     kOpenSquareBracket  = "[";
const std::string_view                     kCloseSquareBracket = "]";
const std::string_view                     kSemicolcon         = ";";
const ast_base_ref                         kMaybeEmpty         = nullptr;

}  // namespace

graphviz_syntax_analyzer::graphviz_syntax_analyzer(std::string_view graphviz)
  : tokenizer_(graphviz)
  , current_token_(tokenizer_.get_next_token()) {
}

ast_base_ref graphviz_syntax_analyzer::attributes_list() {
  return nullptr;
}

ast_base_ref graphviz_syntax_analyzer::node_statement() {
  const auto node_name = get_token_maybe<token::identifier>();
  if (!node_name.has_value()) {
    return kMaybeEmpty;
  }
  return dsac::make_shared<ast_node_node>(node_name.value(), attributes_list());
}

ast_base_ref graphviz_syntax_analyzer::statement() {
  return node_statement();
}

ast_base_ref graphviz_syntax_analyzer::statement_list() {
  auto statements = dsac::make_shared<ast_statement_list_node>();
  for (ast_base_ref stmt = statement(); stmt != kMaybeEmpty; stmt = statement()) {
    statements->insert_statement(stmt);
    if (get_token<token::punctuator>() != kSemicolcon) {
      throw unexpected_syntax{};
    }
  }
  return statements;
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
