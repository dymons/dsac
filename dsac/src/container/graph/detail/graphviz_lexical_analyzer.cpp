#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dsac {

namespace {

const std::unordered_set<std::string>                           kKeywords{"graph", "digraph"};
const std::unordered_map<char, std::unordered_set<std::string>> kOperators{{'-', {"--", "->"}}};
const std::unordered_map<char, std::unordered_set<std::string>> kPunctuations{{';', {";"}}, {'{', {"{"}}, {'}', {"}"}}};

}  // namespace

graphviz_lexical_analyzer::graphviz_lexical_analyzer(std::string_view graphviz)
  : graphviz_(graphviz)
  , m_current_symbol_(0U) {
}

std::pair<token, std::string_view> graphviz_lexical_analyzer::get_next_token() {
  return {};
}

}  // namespace dsac
