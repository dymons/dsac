#pragma once

#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <chrono>
#include <optional>

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
