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

const std::locale    kClassicLocale{"C"};
constexpr const char kUnderlineSymbol{'_'};

}  // namespace

graphviz_lexical_analyzer::graphviz_lexical_analyzer(std::string_view graphviz)
  : graphviz_(graphviz)
  , current_symbol_(0U) {
}

bool graphviz_lexical_analyzer::is_space_then_skip() {
  if (std::isspace(graphviz_[current_symbol_]) != 0) {
    current_symbol_++;
    return true;
  }
  return false;
}

template <token token_type>
std::optional<std::pair<token, std::string_view>> graphviz_lexical_analyzer::try_parse() {
  const auto& entities = token_type == token::operator_ ? kOperators : kPunctuations;

  const auto entity = entities.find(graphviz_[current_symbol_]);
  if (entity != entities.end()) {
    const auto max_combination = std::max_element(entity->second.begin(), entity->second.end());
    auto       max_size =
        std::clamp<decltype(current_symbol_)>(max_combination->size(), 0, graphviz_.size() - current_symbol_);
    do {
      if (auto data = entity->second.find(graphviz_.substr(current_symbol_, max_size)); data != entity->second.end()) {
        current_symbol_ += max_size;
        return std::make_pair(token_type, *data);
      }
    } while (--max_size != 0);
  }
  return std::nullopt;
}

template <>
std::optional<std::pair<token, std::string_view>> graphviz_lexical_analyzer::try_parse<token::keyword>() {
  if (std::isalpha(graphviz_[current_symbol_]) != 0) {
    const auto        current_symbol = static_cast<std::string_view::difference_type>(current_symbol_);
    const char* const not_isalnum =
        std::find_if_not(std::next(graphviz_.begin(), current_symbol), graphviz_.end(), [](char ch) {
          return std::isalnum(ch, kClassicLocale) || (ch == kUnderlineSymbol);
        });
    const std::size_t      end_of_word = std::distance(graphviz_.begin(), not_isalnum);
    const std::string_view word =
        graphviz_.substr(current_symbol_, static_cast<std::string_view::size_type>(end_of_word) - current_symbol_);
    current_symbol_ = static_cast<decltype(current_symbol_)>(std::distance(graphviz_.begin(), not_isalnum));

    if (kKeywords.contains(word)) {
      return std::make_pair(token::keyword, word);
    }
    return std::make_pair(token::identifier, word);
  }

  return std::nullopt;
}

std::pair<token, std::string_view> graphviz_lexical_analyzer::get_next_token() {
  while (!empty()) {
    if (is_space_then_skip()) {
      continue;
    }

    if (auto const keyword = try_parse<token::keyword>(); keyword) {
      return keyword.value();
    } else if (auto const punctuation = try_parse<token::punctuator>(); punctuation) {
      return punctuation.value();
    } else if (auto const operator_ = try_parse<token::operator_>(); operator_) {
      return operator_.value();
    }
  }
  return std::make_pair(token::eof, "");
}

bool graphviz_lexical_analyzer::empty() const noexcept {
  return graphviz_.size() == current_symbol_;
}

}  // namespace dsac
