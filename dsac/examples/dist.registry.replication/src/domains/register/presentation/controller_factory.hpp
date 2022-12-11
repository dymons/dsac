#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <nlohmann/json_fwd.hpp>

namespace dsac::presentation {

class controller {
  executor_base_ref executor_;

protected:
  [[nodiscard]] executor_base_ref get_executor() const noexcept {
    return executor_;
  }

public:
  using factory = ::dsac::factory<controller, executor_base_ref>;

  explicit controller(executor_base_ref executor)
    : executor_(std::move(executor)) {
  }

  controller(const controller&)                = default;
  controller(controller&&) noexcept            = default;
  controller& operator=(const controller&)     = default;
  controller& operator=(controller&&) noexcept = default;
  virtual ~controller()                        = default;

  virtual auto handle(nlohmann::json const& request) -> nlohmann::json = 0;
};

}  // namespace dsac::presentation
