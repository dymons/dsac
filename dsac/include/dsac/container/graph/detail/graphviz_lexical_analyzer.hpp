#pragma once

#include <dsac/container/graph/detail/graphviz_tokens.hpp>

#include <string_view>

namespace dsac {

class graphviz_lexical_analyzer final {
  const std::string_view graphviz_;
  std::size_t            m_current_symbol_;

public:
  explicit graphviz_lexical_analyzer(std::string_view graphviz);

  std::pair<token, std::string_view> get_next_token();
};

}  // namespace dsac
