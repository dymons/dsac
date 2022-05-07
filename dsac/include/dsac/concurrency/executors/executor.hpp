#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {
class base_executor {
public:
  base_executor()                                = default;
  base_executor(const base_executor&)            = default;
  base_executor(base_executor&&)                 = default;
  base_executor& operator=(const base_executor&) = default;
  base_executor& operator=(base_executor&&)      = default;
  virtual ~base_executor()                       = default;

  virtual void submit(task&& task) = 0;
  virtual void join()              = 0;
};

using base_executor_ptr = dsac::shared_ptr<base_executor>;
}  // namespace dsac