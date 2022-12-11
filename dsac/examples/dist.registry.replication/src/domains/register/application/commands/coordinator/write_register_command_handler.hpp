#pragma once

#include <cstdint>

namespace dsac::application::command::coordinator {

struct write_register_command final {
  std::int32_t value{};
  std::size_t  timestamp{};
};

class write_register_command_handler {
public:
  static auto handle(write_register_command const& command) -> void;
};

}  // namespace dsac::application::command::coordinator
