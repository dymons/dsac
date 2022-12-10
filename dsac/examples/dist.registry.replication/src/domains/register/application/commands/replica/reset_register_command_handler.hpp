#pragma once

namespace dsac::application::command::replica {

struct reset_register_command final {};

class reset_register_command_handler {
public:
  static auto handle(reset_register_command const& command) -> void;
};

}  // namespace dsac::application::command::replica
