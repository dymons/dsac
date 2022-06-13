#include <dsac/container/graph/detail/graphviz_lexical_analyzer.hpp>

#include <cctype>
#include <locale>
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
const auto kIsIdentifier = [](char ch) { return std::isalnum(ch, kClassicLocale) || (ch == kUnderlineSymbol); };

}  // namespace

graphviz_lexical_analyzer::graphviz_lexical_analyzer(std::string_view graphviz)
  : graphviz_(graphviz)
  , sp_(0U) {
}

template <token token_type>
tl::expected<std::pair<token, std::string_view>, std::string> graphviz_lexical_analyzer::get_next() {
  const auto& entities = token_type == token::operator_ ? kOperators : kPunctuations;
  const auto  entity   = entities.find(graphviz_[sp_]);
  if (entity != entities.end()) {
    const auto  max_combination = std::max_element(entity->second.begin(), entity->second.end());
    std::size_t max_size        = std::clamp<std::size_t>(max_combination->size(), 0U, graphviz_.size() - sp_);
    do {
      if (auto data = entity->second.find(graphviz_.substr(sp_, max_size)); data != entity->second.end()) {
        sp_ += max_size;
        return std::make_pair(token_type, *data);
      }
    } while (--max_size != 0);
  }
  return tl::make_unexpected<std::string>("");
}

template <>
tl::expected<std::pair<token, std::string_view>, std::string> graphviz_lexical_analyzer::get_next<token::keyword>() {
  if (std::isalpha(graphviz_[sp_]) != 0) {
    const auto* const      it            = std::find_if_not(graphviz_.begin() + sp_, graphviz_.end(), kIsIdentifier);
    const std::size_t      start_of_word = std::exchange(sp_, std::distance(graphviz_.begin(), it));
    const std::string_view word          = graphviz_.substr(start_of_word, sp_ - start_of_word);
    return kKeywords.contains(word) ? std::make_pair(token::keyword, word) : std::make_pair(token::identifier, word);
  }
  return tl::make_unexpected<std::string>("");
}

std::pair<token, std::string_view> graphviz_lexical_analyzer::get_next_token() {
  while (graphviz_.size() != sp_) {
    if (std::isspace(graphviz_[sp_]) != 0) {
      sp_++;
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
