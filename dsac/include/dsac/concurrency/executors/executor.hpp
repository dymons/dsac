#pragma once

#include <dsac/common/non_copyable_and_moveable.hpp>
#include <dsac/concurrency/executors/task.hpp>

namespace dsac {

class iexecutor : private non_copy_able_and_moveable {
public:
  virtual ~iexecutor() = default;

  virtual void submit(task&& task) = 0;
  virtual void join()              = 0;
};

}  // namespace dsac
