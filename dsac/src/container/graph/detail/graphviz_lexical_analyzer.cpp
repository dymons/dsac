#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

#include <algorithm>
#include <cctype>
#include <locale>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dsac {

namespace {

const std::unordered_set<std::string_view>                           kKeywords{"graph", "digraph"};
const std::unordered_map<char, std::unordered_set<std::string_view>> kOperators{{'-', {"--", "->"}}};
const std::unordered_map<char, std::unordered_set<std::string_view>> kPunctuations{
    {';', {";"}}, {'{', {"{"}}, {'}', {"}"}}};

const std::locale                        kClassicLocale{"C"};
constexpr const char                     kUnderlineSymbol{'_'};
const std::pair<token, std::string_view> kTokenFallback{token::eof, ""};

}  // namespace

graphviz_lexical_analyzer::graphviz_lexical_analyzer(std::string_view graphviz)
  : graphviz_(graphviz)
  , current_symbol_(0U) {
}

template <token token_type>
tl::expected<std::pair<token, std::string_view>, std::string> graphviz_lexical_analyzer::get_next() {
  const auto& entities = token_type == token::operator_ ? kOperators : kPunctuations;

  const auto entity = entities.find(graphviz_[current_symbol_]);
  if (entity != entities.end()) {
    const auto  max_combination = std::max_element(entity->second.begin(), entity->second.end());
    std::size_t max_size = std::clamp<std::size_t>(max_combination->size(), 0U, graphviz_.size() - current_symbol_);
    do {
      if (auto data = entity->second.find(graphviz_.substr(current_symbol_, max_size)); data != entity->second.end()) {
        current_symbol_ += max_size;
        return std::make_pair(token_type, *data);
      }
    } while (--max_size != 0);
  }
  return tl::make_unexpected<std::string>("");
}

template <>
tl::expected<std::pair<token, std::string_view>, std::string> graphviz_lexical_analyzer::get_next<token::keyword>() {
  if (std::isalpha(graphviz_[current_symbol_]) != 0) {
    const auto isalnum     = [](char ch) { return std::isalnum(ch, kClassicLocale) || (ch == kUnderlineSymbol); };
    const auto not_isalnum = std::find_if_not(std::next(graphviz_.begin(), current_symbol_), graphviz_.end(), isalnum);

    const std::size_t      end_of_word = std::distance(graphviz_.begin(), not_isalnum);
    const std::string_view word        = graphviz_.substr(current_symbol_, end_of_word - current_symbol_);
    current_symbol_                    = end_of_word;

    return kKeywords.contains(word) ? std::make_pair(token::keyword, word) : std::make_pair(token::identifier, word);
  }
  return tl::make_unexpected<std::string>("");
}

std::pair<token, std::string_view> graphviz_lexical_analyzer::get_next_token() {
  while (graphviz_.size() != current_symbol_) {
    if (std::isspace(graphviz_[current_symbol_]) != 0) {
      current_symbol_++;
      continue;
    }

    return get_next<token::keyword>()
        .or_else([this]([[maybe_unused]] auto&& error) { return get_next<token::punctuator>(); })
        .or_else([this]([[maybe_unused]] auto&& error) { return get_next<token::operator_>(); })
        .value_or(kTokenFallback);
  }
  return kTokenFallback;
}

}  // namespace dsac
