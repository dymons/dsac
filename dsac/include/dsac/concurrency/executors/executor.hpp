#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {
class base_executor {
public:
  virtual ~base_executor() = default;

  virtual void Submit(task&& task) = 0;
  virtual void Join()              = 0;
};

using base_executor_ptr = dsac::shared_ptr<base_executor>;
}  // namespace dsac