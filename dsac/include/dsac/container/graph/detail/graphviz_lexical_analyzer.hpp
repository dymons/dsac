#pragma once

#include <dsac/container/graph/detail/graphviz_tokens.hpp>
#include <dsac/functional/expected.hpp>

#include <string_view>

namespace dsac {

class graphviz_lexical_analyzer final {
  const std::string_view graphviz_;
  std::size_t            sp_;

  template <token>
  expected<std::pair<token, std::string_view>, std::string> get_next();

public:
  explicit graphviz_lexical_analyzer(std::string_view graphviz);

  std::pair<token, std::string_view> get_next_token();
};

}  // namespace dsac
