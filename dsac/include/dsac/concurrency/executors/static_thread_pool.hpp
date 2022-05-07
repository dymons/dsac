#pragma once

#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <thread>
#include <vector>

namespace dsac {
class static_thread_poll final : public executor_base {
public:
  explicit static_thread_poll(std::size_t workers);
  static_thread_poll(const static_thread_poll&)            = delete;
  static_thread_poll(static_thread_poll&&)                 = delete;
  static_thread_poll& operator=(const static_thread_poll&) = delete;
  static_thread_poll& operator=(static_thread_poll&&)      = delete;
  ~static_thread_poll() override                           = default;

  void submit(task&& task) override;
  void join() override;

private:
  void StartWorkerThreads(std::size_t workers);
  void WorkerRoutine();

  std::vector<std::thread>         workers_;
  unbounded_blocking_mpmc_queue<task> tasks_;
};

executor_base_ptr make_static_thread_pool(std::size_t workers);
}  // namespace dsac