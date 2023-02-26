#pragma once

namespace dsac {

class execution_context final {
public:
  auto switch_to(execution_context& target_context) -> void;
};

}  // namespace dsac