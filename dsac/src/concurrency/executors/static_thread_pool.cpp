#include <dsac/concurrency/executors/static_thread_pool.hpp>

#include <thread>

#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac {

namespace {

class static_thread_poll final : public iexecutor {
private:
  /// Current user tasks awaiting execution
  unbounded_blocking_mpmc_queue<task> tasks_;

  /// A collection of worker threads
  dynamic_array<std::thread> workers_;

public:
  ~static_thread_poll() final = default;

  explicit static_thread_poll(std::size_t const workers) {
    start_worker_threads(workers);
  }

  void submit(task&& task) final {
    tasks_.push(std::move(task));
  }

  void join() final {
    for (auto& worker : workers_) {
      tasks_.push({});  // Poison Pill
    }
    for (auto& worker : workers_) {
      worker.join();
    }
    workers_.clear();
  }

private:
  void start_worker_threads(std::size_t workers) {
    for (auto i = std::size_t{}; i < workers; ++i) {
      workers_.emplace_back([this]() { worker_routine(); });
    }
  }

  void worker_routine() {
    while (true) {
      if (auto task = tasks_.pop(); task) {
        task();
      } else {
        break;
      }
    }
  }
};

}  // namespace

auto make_static_thread_pool(std::size_t const workers) -> dsac::shared_ptr<iexecutor> {
  return make_shared<static_thread_poll>(workers);
}

}  // namespace dsac
