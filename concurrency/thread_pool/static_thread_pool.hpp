#pragma once

#include <concurrency/thread_pool/task.hpp>
#include <concurrency/thread_pool/blocking_queue.hpp>
#include <vector>
#include <thread>

namespace algo::concurrency {
class StaticThreadPool final {
 public:
  explicit StaticThreadPool(std::size_t workers);
  StaticThreadPool(const StaticThreadPool&) = delete;
  StaticThreadPool(StaticThreadPool&&) = default;
  StaticThreadPool& operator=(const StaticThreadPool&) = delete;
  StaticThreadPool& operator=(StaticThreadPool&&) = default;
  ~StaticThreadPool() = default;

  void Submit(Task task);
  void Join();

 private:
  void StartWorkerThreads(std::size_t workers);
  void WorkerRoutine();

 private:
  std::vector<std::thread> workers_;
  UnboundedBlockingMPMCQueue<Task> tasks_;
};
}  // namespace algo::concurrency