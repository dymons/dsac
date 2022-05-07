#include <dsac/concurrency/executors/blocking_queue.hpp>
#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <dsac/concurrency/executors/task.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <thread>

namespace dsac::detail {

class static_thread_poll final : public executor_base {
public:
  /*!
    \brief
        User constructor, constructs a thread pool with fixed number of workers.
  */
  explicit static_thread_poll(std::size_t workers);

  /*!
    \brief
        Copy constructor.
  */
  static_thread_poll(const static_thread_poll&) = delete;

  /*!
    \brief
        Move constructor.
  */
  static_thread_poll(static_thread_poll&&) = delete;

  /*!
    \brief
        Copy conversion constructor.
  */
  static_thread_poll& operator=(const static_thread_poll&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  static_thread_poll& operator=(static_thread_poll&&) = delete;

  /*!
    \brief
        Destructor.
  */
  ~static_thread_poll() override = default;

  void submit(task&& task) override;
  void join() override;

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

  /// A collection of worker threads
  dynamic_array<std::thread> workers_;

  /// Current user tasks awaiting execution
  unbounded_blocking_mpmc_queue<task> tasks_;
};

static_thread_poll::static_thread_poll(std::size_t workers) {
  start_worker_threads(workers);
}

void static_thread_poll::start_worker_threads(std::size_t workers) {
  for (std::size_t i{}; i < workers; ++i) {
    workers_.emplace_back([this]() { worker_routine(); });
  }
}

void static_thread_poll::worker_routine() {
  while (true) {
    if (task task = tasks_.pop(); task) {
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

}  // namespace dsac::detail

namespace dsac {

executor_base_ptr make_static_thread_pool(std::size_t workers) {
  return make_shared<detail::static_thread_poll>(workers);
}

}  // namespace dsac
