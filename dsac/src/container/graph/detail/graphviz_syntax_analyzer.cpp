#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

#include <unordered_set>

namespace dsac {

namespace {

const std::string_view                     kDirectGraph   = "digraph";
const std::string_view                     kUndirectGraph = "graph";
const std::unordered_set<std::string_view> kGraphTypes{kUndirectGraph, kDirectGraph};
const std::string_view                     kDirectConnection   = "->";
const std::string_view                     kUndirectConnection = "--";
const std::unordered_set<std::string_view> kConnectionOperator{kDirectConnection, kUndirectConnection};
const std::string_view                     kAssignOperator     = "=";
const std::string_view                     kOpenCurlyBracket   = "{";
const std::string_view                     kCloseCurlyBracket  = "}";
const std::string_view                     kOpenSquareBracket  = "[";
const std::string_view                     kCloseSquareBracket = "]";
const std::string_view                     kSemicolcon         = ";";
const ast_base_ref                         kEmpty              = nullptr;

[[gnu::always_inline]] inline kind get_connection_kind(std::string_view connection) {
  if (kConnectionOperator.contains(connection)) {
    return connection == kDirectConnection ? kind::edge_direct_connection : kind::edge_undirect_connection;
  }
  throw unexpected_syntax{};
}

ast_base_ref make_edge_by_kind(kind kind, ast_base_ref from, ast_base_ref to) {
  switch (kind) {
    case kind::edge_undirect_connection: {
      return dsac::make_shared<ast_undirect_edge_node>(from, to);
    }
    case kind::edge_direct_connection: {
      return dsac::make_shared<ast_direct_edge_node>(from, to);
    }
  }
  throw unexpected_syntax{};
}

[[gnu::always_inline]] inline kind get_graph_kind(std::string_view graph) {
  if (kGraphTypes.contains(graph)) {
    return graph == kDirectGraph ? kind::direct_graph : kind::undirect_graph;
  }
  throw unexpected_syntax{};
}

ast_base_ref make_graph_by_kind(kind kind, std::string_view graph_name, ast_base_ref child) {
  switch (kind) {
    case kind::direct_graph: {
      return dsac::make_shared<ast_direct_graph_node>(graph_name, child);
    }
    case kind::undirect_graph: {
      return dsac::make_shared<ast_undirect_graph_node>(graph_name, child);
    }
  }
  throw unexpected_syntax{};
}

}  // namespace

graphviz_syntax_analyzer::graphviz_syntax_analyzer(std::string_view graphviz)
  : tokenizer_(graphviz)
  , current_token_(tokenizer_.get_next_token()) {
}

ast_base_ref graphviz_syntax_analyzer::attribute() {
  if (current_token_.first != token::identifier) {
    return kEmpty;
  }
  const std::string_view key = get_token<token::identifier>();
  if (get_token<token::operator_>() != kAssignOperator) {
    throw unexpected_syntax{};
  }
  return dsac::make_shared<ast_attribute_node>(
      kind::attribute,
      key,
      get_token_maybe<token::identifier>()
          .or_else([this]([[maybe_unused]] auto&& error) { return get_token_maybe<token::literal>(); })
          .or_else([]([[maybe_unused]] auto&& error) __attribute__((__noreturn__)) { throw unexpected_syntax{}; })
          .value()
  );
}

ast_base_ref graphviz_syntax_analyzer::attributes_list() {
  if (current_token_.second != kOpenSquareBracket) {
    return kEmpty;
  }
  [[maybe_unused]] const auto open_square_bracket = get_token<token::punctuator>();

  auto attributes = dsac::make_shared<ast_attributes_node>(kind::attributes);
  for (ast_base_ref attr = attribute(); attr != kEmpty; attr = attribute()) {
    attributes->insert_attribute(std::move(attr));
  }

  if (get_token<token::punctuator>() != kCloseSquareBracket) {
    throw unexpected_syntax{};
  }

  return attributes;
}

ast_base_ref graphviz_syntax_analyzer::node_statement() {
  const auto node_name = get_token_maybe<token::identifier>();
  if (!node_name.has_value()) {
    return kEmpty;
  }
  return dsac::make_shared<ast_node_node>(kind::node, node_name.value(), attributes_list());
}

ast_base_ref graphviz_syntax_analyzer::statement() {
  ast_base_ref left_node = node_statement();
  if (auto operator_ = get_token_maybe<token::operator_>(); operator_.has_value()) {
    if (!kConnectionOperator.contains(operator_.value())) {
      throw unexpected_syntax{};
    }
    kind const connection_kind = get_connection_kind(*operator_);
    return make_edge_by_kind(connection_kind, left_node, statement());
  }
  return left_node;
}

ast_base_ref graphviz_syntax_analyzer::statement_list() {
  auto statements = dsac::make_shared<ast_statements_node>(kind::statements);
  for (ast_base_ref stmt = statement(); stmt != kEmpty; stmt = statement()) {
    statements->insert_statement(std::move(stmt));
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
  kind const   graph_kind = get_graph_kind(graph_type);
  ast_base_ref graph_root = make_graph_by_kind(graph_kind, graph_name, statement_list());
  if (get_token<token::punctuator>() != kCloseCurlyBracket) {
    throw unexpected_syntax{};
  }

  return graph_root;
}

ast_base_ref graphviz_syntax_analyzer::parse() {
  return graph();
}

}  // namespace dsac
