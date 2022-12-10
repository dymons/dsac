#pragma once

#include <dsac/pattern/registrator/registrator.hpp>

#include <nlohmann/json_fwd.hpp>

namespace dsac::presentation {

class controller {
public:
  using factory = ::dsac::factory<controller>;

  controller()                                 = default;
  controller(const controller&)                = default;
  controller(controller&&) noexcept            = default;
  controller& operator=(const controller&)     = default;
  controller& operator=(controller&&) noexcept = default;
  virtual ~controller()                        = default;

  virtual auto handle(nlohmann::json const& request) -> nlohmann::json = 0;
};

}  // namespace dsac::presentation
