#include <dsac/concurrency/executors/static_thread_pool.hpp>

namespace dsac::concurrency {
StaticThreadPool::StaticThreadPool(std::size_t workers) {
  StartWorkerThreads(workers);
}

void StaticThreadPool::StartWorkerThreads(std::size_t workers) {
  for (std::size_t i{}; i < workers; ++i) {
    workers_.emplace_back([this]() {
      WorkerRoutine();
    });
  }
}

void StaticThreadPool::WorkerRoutine() {
  while (true) {
    if (Task task = tasks_.Pop(); task) {
      task();
    } else {
      break;
    }
  }
}

IExecutorPtr StaticThreadPool::Make(std::size_t workers) {
  return std::make_shared<StaticThreadPool>(workers);
}

void StaticThreadPool::Submit(Task&& task) {
  tasks_.Push(std::move(task));
}

void StaticThreadPool::Join() {
  for (auto& worker : workers_) {
    tasks_.Push({});  // Poison Pill
  }
  for (auto& worker : workers_) {
    worker.join();
  }
  workers_.clear();
}
}  // namespace dsac::concurrency