#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <thread>

namespace dsac {

namespace {

class static_thread_poll final : public iexecutor {
public:
  /*!
    \brief
        User constructor, constructs a thread pool with fixed number of workers.
  */
  explicit static_thread_poll(const std::size_t workers);

  /*!
    \brief
        Destructor.
  */
  ~static_thread_poll() final = default;

  void submit(task&& task) final;
  void join() final;

private:
  /*!
    \brief
        Initialize and run threads.
  */
  void start_worker_threads(std::size_t workers);

  /*!
    \brief
        Main routine handler in the thread pool.
  */
  void worker_routine();

  /// Current user tasks awaiting execution
  unbounded_blocking_mpmc_queue<task> tasks_;

  /// A collection of worker threads
  dynamic_array<std::thread> workers_;
};

static_thread_poll::static_thread_poll(std::size_t const workers) {
  start_worker_threads(workers);
}

void static_thread_poll::start_worker_threads(std::size_t const workers) {
  for (auto i = std::size_t{}; i < workers; ++i) {
    workers_.emplace_back([this]() { worker_routine(); });
  }
}

void static_thread_poll::worker_routine() {
  while (true) {
    if (auto task = tasks_.pop(); task) {
      task();
    } else {
      break;
    }
  }
}

void static_thread_poll::submit(task&& task) {
  tasks_.push(std::move(task));
}

void static_thread_poll::join() {
  for (auto& worker : workers_) {
    tasks_.push({});  // Poison Pill
  }
  for (auto& worker : workers_) {
    worker.join();
  }
  workers_.clear();
}

}  // namespace

auto make_static_thread_pool(std::size_t const workers) -> dsac::shared_ptr<iexecutor> {
  return make_shared<static_thread_poll>(workers);
}

}  // namespace dsac
