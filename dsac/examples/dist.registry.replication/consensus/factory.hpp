#pragma once

#include <examples/dist.registry.replication//models/request.hpp>
#include <examples/dist.registry.replication//models/response.hpp>

#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

namespace dsac {

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
