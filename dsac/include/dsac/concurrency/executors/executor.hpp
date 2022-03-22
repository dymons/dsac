#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <memory>

namespace dsac::concurrency {
class IExecutor {
public:
  virtual ~IExecutor() = default;

  virtual void Submit(task&& task) = 0;
  virtual void Join()              = 0;
};

using IExecutorPtr = std::shared_ptr<IExecutor>;
}  // namespace dsac::concurrency