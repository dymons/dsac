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

const std::locale              kClassicLocale{"C"};
constexpr const char           kUnderlineSymbol{'_'};
const std::unordered_set<char> kDigitSymbols{'+', '-', '.'};

}  // namespace

graphviz_lexical_analyzer::graphviz_lexical_analyzer(std::string_view graphviz)
  : graphviz_(graphviz)
  , current_symbol_(0U) {
}

std::pair<token, std::string_view> graphviz_lexical_analyzer::get_next_token() {
  while (!empty()) {
    if (std::isspace(graphviz_[current_symbol_]) != 0) {
      current_symbol_++;
      continue;
    }

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
    } else if (std::isdigit(graphviz_[current_symbol_], kClassicLocale)) {
      const auto  current_pose_symbol = static_cast<std::string_view::difference_type>(current_symbol_);
      const char* not_isdigit =
          std::find_if_not(std::next(graphviz_.begin(), current_pose_symbol), graphviz_.end(), [](char ch) {
            return (std::isdigit(ch, kClassicLocale) || kDigitSymbols.contains(ch));
          });

      if (not_isdigit != graphviz_.end()) {
        const auto       end_of_digital = std::distance(graphviz_.begin(), not_isdigit);
        std::string_view digital =
            graphviz_.substr(current_symbol_, static_cast<std::string::size_type>(end_of_digital) - current_symbol_);
        current_symbol_ = static_cast<std::size_t>(std::distance(graphviz_.begin(), not_isdigit));
        return {token::literal, digital};
      }

      auto digital = graphviz_.substr(current_symbol_, graphviz_.size() - current_symbol_);
      return {token::literal, digital};
    }
    const auto punctuation = kPunctuations.find(graphviz_[current_symbol_]);
    if (punctuation != kPunctuations.end()) {
      const auto max_combination = std::max_element(punctuation->second.begin(), punctuation->second.end());
      auto       max_size =
          std::clamp<decltype(current_symbol_)>(max_combination->size(), 0, graphviz_.size() - current_symbol_);
      do {
        if (auto punct = punctuation->second.find(graphviz_.substr(current_symbol_, max_size));
            punct != punctuation->second.end()) {
          current_symbol_ += max_size;
          return {token::punctuator, *punct};
        }
      } while (--max_size != 0);
    }

    const auto operators = kOperators.find(graphviz_[current_symbol_]);
    if (operators != kOperators.end()) {
      const auto max_combination = std::max_element(operators->second.begin(), operators->second.end());
      auto       max_size =
          std::clamp<decltype(current_symbol_)>(max_combination->size(), 0, graphviz_.size() - current_symbol_);
      do {
        if (auto oper = operators->second.find(graphviz_.substr(current_symbol_, max_size));
            oper != operators->second.end()) {
          current_symbol_ += max_size;
          return {token::operator_, *oper};
        }
      } while (--max_size != 0);
    }

    switch (graphviz_[current_symbol_]) {
      case '"': {
        if (const auto last_mark = graphviz_.find_first_of('"', current_symbol_ + 1); last_mark != std::string::npos) {
          current_symbol_ = last_mark + 1;
          return {token::literal, graphviz_.substr(current_symbol_, last_mark - current_symbol_)};
        }

        return {token::punctuator, std::string{graphviz_[current_symbol_++]}};
      }
      default: {
        throw std::logic_error{""};
      }
    }
  }
  return std::make_pair(token::eof, "");
}

bool graphviz_lexical_analyzer::empty() const noexcept {
  return graphviz_.size() == current_symbol_;
}

}  // namespace dsac
