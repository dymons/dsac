#pragma once

#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <chrono>
#include <optional>

#include <nlohmann/json_fwd.hpp>

namespace dsac {

struct request final {
  std::optional<std::string>                                        key;
  std::optional<std::string>                                        value;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> timestamp;
};

struct response final {
  std::optional<std::string>                                        value;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> timestamp;
};

class parse_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

template <typename Message>
auto from_json(nlohmann::json const& j, Message& r) -> void;

template <typename Message>
auto to_json(Message const& r) -> nlohmann::json;

class abd {
public:
  using factory = ::dsac::factory<abd>;

  abd()                          = default;
  abd(const abd&)                = default;
  abd(abd&&) noexcept            = default;
  abd& operator=(const abd&)     = default;
  abd& operator=(abd&&) noexcept = default;
  virtual ~abd()                 = default;

  virtual auto execute(request request) -> expected<response, std::string> = 0;
};

}  // namespace dsac
