#pragma once

namespace dsac {

class non_copy_able_and_moveable {
public:
  non_copy_able_and_moveable()                                             = default;
  non_copy_able_and_moveable(non_copy_able_and_moveable const&)            = delete;
  non_copy_able_and_moveable(non_copy_able_and_moveable&&)                 = delete;
  non_copy_able_and_moveable& operator=(non_copy_able_and_moveable const&) = delete;
  non_copy_able_and_moveable& operator=(non_copy_able_and_moveable&&)      = delete;
};

}  // namespace dsac
