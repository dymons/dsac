#pragma once

#include <span>

#include <dsac/concurrency/fibers/trampoline.hpp>

namespace dsac {

class execution_context final {
  class machine_context final {
    void* rsp_{};

  public:
    // Modifiers

    auto setup(std::span<char> stack, trampoline_base* trampoline) -> void;

    auto switch_to(machine_context& target_context) -> void;
  };

  machine_context machine_context_;

public:
  // Modifiers

  auto setup(std::span<char> stack, trampoline_base* trampoline) -> void;

  auto switch_to(execution_context& target_context) -> void;
};

}  // namespace dsac