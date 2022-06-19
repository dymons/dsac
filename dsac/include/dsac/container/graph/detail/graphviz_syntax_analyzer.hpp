#pragma once

#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

namespace dsac {

class graphviz_analyzer_analyzer final {
  graphviz_lexical_analyzer tokenizer_;

public:
  explicit graphviz_analyzer_analyzer(std::string_view graphviz);
};

}  // namespace dsac
