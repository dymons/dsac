#include <dsac/concurrency/fibers/details/execution_context/execution_context.hpp>
#include <dsac/concurrency/fibers/details/fiber.hpp>
#include <dsac/concurrency/fibers/details/scheduler.hpp>
#include <dsac/concurrency/fibers/details/stack/stack.hpp>
#include <dsac/container/intrusive/list.hpp>

namespace {

thread_local dsac::fiber_scheduler* kScheduler{};

}  // namespace

namespace dsac {

class fiber_scheduler::fiber_scheduler_pimpl final {
  auto switch_to(fiber* fiber) -> void {
    current_fiber_ = fiber;
    execution_context_.switch_to(current_fiber_->get_execution_context());
    current_fiber_ = nullptr;
  }

  auto dispatch_of(fiber* fiber) -> void {
    delete fiber;
  }

public:
  auto main(fiber_routine entry_routine) -> void {
    submit_routing(std::move(entry_routine));
    while (not fibers_.empty()) {
      fiber* fiber = fibers_.pop_front();
      switch_to(fiber);
      dispatch_of(fiber);
    }
  }

  auto terminate() -> void {
    stacks_.release(std::move(current_fiber_->get_stack()));
    current_fiber_->get_execution_context().switch_to(execution_context_);
  }

  auto submit_routing(fiber_routine routine) -> void {
    fibers_.push_back(fiber::make(kScheduler, stacks_.allocate(), std::move(routine)));
  }

private:
  fiber*                 current_fiber_{};
  intrusive::list<fiber> fibers_{};
  execution_context      execution_context_{};
  fiber_stack_allocator  stacks_{};
};

auto fiber_scheduler::make() -> fiber_scheduler {
  return {};
}

fiber_scheduler::fiber_scheduler()
  : pimpl_(make_shared<fiber_scheduler::fiber_scheduler_pimpl>()) {
}

auto fiber_scheduler::running_entry_routing(fiber_routine entry_routine) -> void {
  kScheduler = this;
  kScheduler->pimpl_->main(std::move(entry_routine));
  kScheduler = nullptr;
}

auto fiber_scheduler::terminate() -> void {
  pimpl_->terminate();
}

auto fiber_scheduler::submit_routing(fiber_routine routine) -> void {
  pimpl_->submit_routing(std::move(routine));
}

fiber_scheduler* fiber_scheduler::get_current_scheduler() {
  return kScheduler;
}

}  // namespace dsac