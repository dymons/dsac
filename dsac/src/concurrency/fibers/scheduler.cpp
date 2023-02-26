#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/concurrency/fibers/stack/stack_allocator.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler;

}  // namespace

namespace dsac {

class fiber_scheduler::fiber_scheduler_pimpl final {
  auto schedule_to(fiber* fiber) -> auto* {
    struct fiber_scope_guard final {
      fiber_scheduler_pimpl* scheduler;

      fiber_scope_guard(fiber_scheduler_pimpl* scheduler, class fiber* fiber) noexcept
        : scheduler(scheduler) {
        scheduler->current_fiber_ = fiber;
        scheduler->current_fiber_->set_state(fiber_state::running);
      }
      ~fiber_scope_guard() noexcept {
        scheduler->current_fiber_ = nullptr;
      }
    };

    auto fiber_guard = fiber_scope_guard{this, fiber};
    switch_to_fiber(fiber);
    return fiber;
  }

  auto switch_to_fiber(fiber* fiber) -> void {
    execution_context_.switch_to(fiber->get_execution_context());
  }

  auto dispatch(fiber* fiber) -> void {
    delete fiber;
  }

public:
  auto schedule(fiber_routine entry_routine) & -> void {
    fiber_queue_.push_back(fiber::make(std::move(entry_routine)));
    while (not fiber_queue_.empty()) {
      dispatch(schedule_to(fiber_queue_.pop_front()));
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
  struct fiber_scheduler_scope_guard final {
    explicit fiber_scheduler_scope_guard(fiber_scheduler* scheduler) noexcept {
      kScheduler = scheduler;
    }
    ~fiber_scheduler_scope_guard() noexcept {
      kScheduler = nullptr;
    }

    auto operator->() noexcept -> fiber_scheduler* {
      return kScheduler;
    }
  };

  fiber_scheduler_scope_guard(this)->pimpl_->schedule(std::move(entry_routine));
}

auto fiber_scheduler::submit(fiber_routine routine) -> void {
  pimpl_->submit(std::move(routine));
}

}  // namespace dsac