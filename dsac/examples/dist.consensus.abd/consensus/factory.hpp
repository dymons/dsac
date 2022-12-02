#pragma once

#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <nlohmann/json.hpp>

namespace dsac {

using nlohmann::json;

class abd {
public:
  using factory = ::dsac::factory<abd>;

  abd()                          = default;
  abd(const abd&)                = default;
  abd(abd&&) noexcept            = default;
  abd& operator=(const abd&)     = default;
  abd& operator=(abd&&) noexcept = default;
  virtual ~abd()                 = default;

  virtual auto execute(json request) -> expected<json, std::string> = 0;
};

}  // namespace dsac
