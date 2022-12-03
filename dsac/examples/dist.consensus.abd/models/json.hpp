#pragma once

#include <nlohmann/json_fwd.hpp>

namespace dsac {

class parse_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

template <typename Message>
auto from_json(nlohmann::json const& j, Message& r) -> void;

template <typename Message>
auto to_json(Message const& r) -> nlohmann::json;

}  // namespace dsac