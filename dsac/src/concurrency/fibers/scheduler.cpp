#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/concurrency/fibers/stack/stack_allocator.hpp>
#include <dsac/container/intrusive/list.hpp>
#include <dsac/functional/defer.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler;

}  // namespace

namespace dsac {

class fiber_scheduler::fiber_scheduler_pimpl final {
  auto switch_to(fiber* fiber) -> void {
    auto defer_fiber = defer([this]() noexcept { this->current_fiber_ = nullptr; });

    current_fiber_ = fiber;
    current_fiber_->set_state(fiber_state::running);
    execution_context_.switch_to(current_fiber_->get_execution_context());
  }

  auto dispatch_of(fiber* fiber) -> void {
    delete fiber;
  }

public:
  auto schedule(fiber_routine entry_routine) & -> void {
    fiber_queue_.push_back(fiber::make(std::move(entry_routine)));
    while (not fiber_queue_.empty()) {
      fiber* fiber = fiber_queue_.pop_front();
      switch_to(fiber);
      dispatch_of(fiber);
    }
  }

  auto submit(fiber_routine routine) -> void {
    fiber_queue_.push_back(fiber::make(std::move(routine)));
  }

private:
  fiber*                 current_fiber_{};
  intrusive::list<fiber> fiber_queue_{};
  execution_context      execution_context_{};
};

fiber_scheduler::fiber_scheduler()
  : pimpl_(make_shared<fiber_scheduler::fiber_scheduler_pimpl>()) {
}

auto fiber_scheduler::schedule(fiber_routine entry_routine) -> void {
  auto defer_scheduler = defer([this]() noexcept { kScheduler = nullptr; });

  kScheduler = this;
  kScheduler->pimpl_->schedule(std::move(entry_routine));
}

auto fiber_scheduler::submit(fiber_routine routine) -> void {
  pimpl_->submit(std::move(routine));
}

}  // namespace dsac