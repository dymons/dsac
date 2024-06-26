#include <dsac/pattern/registrator/registrator.hpp>

#include <fmt/format.h>

namespace dsac {

factory_component_duplicate::factory_component_duplicate(const std::string& component_name)
  : factory_exception(fmt::format("Component '{}' already exist", component_name)) {
}

}  // namespace dsac
