#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/detail/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

namespace dsac::infrastructure::inmemory {

auto register_repository::write(domain::register_value_object const& object) -> void {
  return singleton<detail::register_>()->write(object);
}

auto register_repository::reset() -> void {
  return singleton<detail::register_>()->reset();
}

auto register_repository::read() -> std::optional<domain::register_value_object> {
  return singleton<detail::register_>()->read();
}

}  // namespace dsac::infrastructure::inmemory