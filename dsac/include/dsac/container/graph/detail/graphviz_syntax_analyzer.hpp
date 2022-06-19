#pragma once

#include <dsac/container/graph/detail/graphviz_abstract_syntax_tree.hpp>
#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

namespace dsac {

class graphviz_syntax_analyzer_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class unexpected_syntax : public graphviz_syntax_analyzer_exception {
public:
  unexpected_syntax()
    : graphviz_syntax_analyzer_exception("The syntax does not match with dot language") {
  }
};

class graphviz_syntax_analyzer final {
  graphviz_lexical_analyzer          tokenizer_;
  std::pair<token, std::string_view> current_token_;

  template <token expected_token>
  std::string_view get_token();

  /*!
    \brief
        statement SEMICOLCON statement_list | empty
  */
  ast_base_ref statement_list();

  /*!
    \brief
        (graph | digraph) IDENTIFIER LPARAM statement_list RPARAM
  */
  ast_base_ref graph();

public:
  explicit graphviz_syntax_analyzer(std::string_view graphviz);

  ast_base_ref parse();
};

template <token expected_token>
std::string_view graphviz_syntax_analyzer::get_token() {
  if (current_token_.first != expected_token) {
    throw unexpected_syntax{};
  }
  return std::exchange(current_token_, tokenizer_.get_next_token()).second;
}

}  // namespace dsac
