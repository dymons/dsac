#pragma once

#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <thread>
#include <vector>

namespace dsac::concurrency {
class StaticThreadPool final : public IExecutor {
public:
  static IExecutorPtr Make(std::size_t workers);

  explicit StaticThreadPool(std::size_t workers);
  StaticThreadPool(const StaticThreadPool&)            = delete;
  StaticThreadPool(StaticThreadPool&&)                 = delete;
  StaticThreadPool& operator=(const StaticThreadPool&) = delete;
  StaticThreadPool& operator=(StaticThreadPool&&)      = delete;
  ~StaticThreadPool() override                         = default;

  void Submit(task&& task) override;
  void Join() override;

private:
  void StartWorkerThreads(std::size_t workers);
  void WorkerRoutine();

private:
  std::vector<std::thread>         workers_;
  UnboundedBlockingMPMCQueue<task> tasks_;
};
}  // namespace dsac::concurrency