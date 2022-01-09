#pragma once

#include <concurrency/executors/task.hpp>
#include <memory>

namespace algo::concurrency {
class IExecutor {
 public:
  virtual ~IExecutor() = default;

  virtual void Submit(Task task) = 0;
  virtual void Join() = 0;
};

using IExecutorPtr = std::shared_ptr<IExecutor>;
}  // namespace algo::concurrency