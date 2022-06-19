#include <dsac/container/graph/detail/graphviz_syntax_analyzer.hpp>

namespace dsac {

graphviz_analyzer_analyzer::graphviz_analyzer_analyzer(std::string_view graphviz)
  : tokenizer_(graphviz) {
}

}  // namespace dsac
