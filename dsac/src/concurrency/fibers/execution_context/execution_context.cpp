#include <dsac/concurrency/fibers/execution_context/execution_context.hpp>

namespace dsac {

namespace {

extern "C" void* setup_machine_context(void* stack, void* trampoline, void* arg);
extern "C" void  switch_machine_context(void* from_rsp, void* to_rsp);

void trampoline_executor(
    void* /*unused*/,
    void* /*unused*/,
    void* /*unused*/,
    void* /*unused*/,
    void* /*unused*/,
    void* /*unused*/,
    void* arg7
) {
  static_cast<trampoline_base*>(arg7)->run();
}

}  // namespace

auto execution_context::machine_context::setup(std::span<char> stack_view, trampoline_base* trampoline) -> void {
  auto* end_of_stack = static_cast<void*>(stack_view.data() + stack_view.size());
  rsp_               = setup_machine_context(end_of_stack, reinterpret_cast<void*>(trampoline_executor), trampoline);
}

auto execution_context::machine_context::switch_to(machine_context& target_context) -> void {
  switch_machine_context(&rsp_, &target_context.rsp_);
}

auto execution_context::setup(std::span<char> stack, trampoline_base* trampoline) -> void {
  machine_context_.setup(stack, trampoline);
}

auto execution_context::switch_to(execution_context& target_context) -> void {
  machine_context_.switch_to(target_context.machine_context_);
}

}  // namespace dsac
