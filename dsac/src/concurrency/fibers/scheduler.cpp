#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/fiber.hpp>
#include <dsac/concurrency/fibers/scheduler.hpp>
#include <dsac/concurrency/fibers/stack/stack_allocator.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler;

}  // namespace

namespace dsac {

class fiber_scheduler::fiber_scheduler_pimpl {
public:
  auto main(fiber_routine entry_routine) & -> void {
    submit(std::move(entry_routine));
    while (not fiber_queue_.empty()) {
      delete fiber_queue_.pop_front();
    }
  }

  auto submit(fiber_routine routine) -> void {
    fiber_queue_.push_back(new fiber{std::move(routine)});
  }

private:
  intrusive::list<fiber>  fiber_queue_;
  fiber_stack_allocator   fiber_stack_allocator_;
  fiber_execution_context fiber_execution_context_;
};

fiber_scheduler::fiber_scheduler()
  : pimpl_(make_shared<fiber_scheduler::fiber_scheduler_pimpl>()) {
}

auto fiber_scheduler::main(fiber_routine entry_routine) & -> void {
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
  
  fiber_scheduler_scope_guard(this)->pimpl_->main(std::move(entry_routine));
}

auto fiber_scheduler::submit(fiber_routine routine) -> void {
  pimpl_->submit(std::move(routine));
}

}  // namespace dsac