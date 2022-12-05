#pragma once

#include <dsac/concurrency/executors/executor.hpp>

namespace dsac {

struct dependencies final {
  executor_base_ref executor;
};

}  // namespace dsac
