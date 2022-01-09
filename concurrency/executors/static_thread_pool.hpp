#pragma once

#include <concurrency/executors/executor.hpp>
#include <concurrency/executors/task.hpp>
#include <concurrency/executors/blocking_queue.hpp>
#include <vector>
#include <thread>

namespace algo::concurrency {
class StaticThreadPool final : public IExecutor {
 public:
  static IExecutorPtr Make(std::size_t workers);

  explicit StaticThreadPool(std::size_t workers);
  StaticThreadPool(const StaticThreadPool&) = delete;
  StaticThreadPool(StaticThreadPool&&) = delete;
  StaticThreadPool& operator=(const StaticThreadPool&) = delete;
  StaticThreadPool& operator=(StaticThreadPool&&) = delete;
  ~StaticThreadPool() override = default;

  void Submit(Task task) override;
  void Join() override;

 private:
  void StartWorkerThreads(std::size_t workers);
  void WorkerRoutine();

 private:
  std::vector<std::thread> workers_;
  UnboundedBlockingMPMCQueue<Task> tasks_;
};
}  // namespace algo::concurrency