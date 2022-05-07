#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {
class executor_base {
public:
  executor_base()                                = default;
  executor_base(const executor_base&)            = default;
  executor_base(executor_base&&)                 = default;
  executor_base& operator=(const executor_base&) = default;
  executor_base& operator=(executor_base&&)      = default;
  virtual ~executor_base()                       = default;

  virtual void submit(task&& task) = 0;
  virtual void join()              = 0;
};

using executor_base_ptr = dsac::shared_ptr<executor_base>;
}  // namespace dsac