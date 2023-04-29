#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/concurrency/fibers/stack/mmap_allocator.hpp>
#include <dsac/concurrency/fibers/stack/stack.hpp>
#include <dsac/container/intrusive/list.hpp>
#include <dsac/functional/defer.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler{};

}  // namespace

namespace dsac {

class fiber_scheduler::fiber_scheduler_pimpl final {
  auto switch_to(fiber* fiber) -> void {
    [[maybe_unused]] auto defer_fiber = defer([this]() noexcept { this->scheduler_current_fiber_ = nullptr; });

    scheduler_current_fiber_ = fiber;
    scheduler_current_fiber_->set_state(fiber_state::running);
    scheduler_execution_context_.switch_to(scheduler_current_fiber_->get_execution_context());
  }

  auto dispatch_of(fiber* fiber) -> void {
    delete fiber;
  }

  [[gnu::always_inline]] auto make_fiber(fiber_routine&& entry_routine) -> fiber* {
    return fiber::make(kScheduler, scheduler_stack_allocator_.get_free_stack(), std::move(entry_routine));
  }

public:
  auto running_entry_routing(fiber_routine&& entry_routine) & -> void {
    scheduler_fiber_queue_.push_back(make_fiber(std::move(entry_routine)));
    while (not scheduler_fiber_queue_.empty()) {
      fiber* fiber = scheduler_fiber_queue_.pop_front();
      switch_to(fiber);
      dispatch_of(fiber);
    }
  }

  auto submit_routine(fiber_routine&& routine) -> void {
    scheduler_fiber_queue_.push_back(make_fiber(std::move(routine)));
  }

private:
  fiber*                 scheduler_current_fiber_{};
  intrusive::list<fiber> scheduler_fiber_queue_{};
  execution_context      scheduler_execution_context_{};
  fiber_stack_allocator  scheduler_stack_allocator_{};
};

auto fiber_scheduler::make() -> fiber_scheduler {
  return {};
}

fiber_scheduler::fiber_scheduler()
  : pimpl_(make_shared<fiber_scheduler::fiber_scheduler_pimpl>()) {
}

auto fiber_scheduler::running_entry_routing(fiber_routine entry_routine) -> void {
  [[maybe_unused]] auto defer_scheduler = defer([]() noexcept { kScheduler = nullptr; });

  kScheduler = this;
  kScheduler->pimpl_->running_entry_routing(std::move(entry_routine));
}

auto fiber_scheduler::submit_routine(fiber_routine routine) -> void {
  pimpl_->submit_routine(std::move(routine));
}

auto fiber_scheduler::terminate() -> void {
}

}  // namespace dsac