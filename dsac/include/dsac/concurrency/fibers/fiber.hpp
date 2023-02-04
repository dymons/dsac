#pragma once

#include <dsac/container/intrusive/list.hpp>

namespace dsac {

class fiber final : public intrusive::list_node_base<fiber> {};

}  // namespace dsac
