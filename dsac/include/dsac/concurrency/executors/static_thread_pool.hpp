#pragma once

#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <thread>
#include <vector>

namespace dsac {
class static_thread_poll final : public base_executor {
public:
  explicit static_thread_poll(std::size_t workers);
  static_thread_poll(const static_thread_poll&)            = delete;
  static_thread_poll(static_thread_poll&&)                 = delete;
  static_thread_poll& operator=(const static_thread_poll&) = delete;
  static_thread_poll& operator=(static_thread_poll&&)      = delete;
  ~static_thread_poll() override                           = default;

  void Submit(task&& task) override;
  void Join() override;

private:
  void StartWorkerThreads(std::size_t workers);
  void WorkerRoutine();

  std::vector<std::thread>         workers_;
  UnboundedBlockingMPMCQueue<task> tasks_;
};

base_executor_ptr make_static_thread_pool(std::size_t workers);
}  // namespace dsac