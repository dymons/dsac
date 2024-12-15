#pragma once

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <nlohmann/json_fwd.hpp>

namespace dsac::presentation {

class controller {
  shared_ptr<iexecutor> executor_;

protected:
  [[nodiscard]] shared_ptr<iexecutor> get_executor() const noexcept {
    return executor_;
  }

public:
  using factory = ::dsac::factory<controller, shared_ptr<iexecutor>>;

  explicit controller(shared_ptr<iexecutor> executor)
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
