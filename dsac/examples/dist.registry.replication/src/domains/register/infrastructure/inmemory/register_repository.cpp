#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/detail/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/register_repository.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

namespace dsac::infrastructure::inmemory {

auto register_repository::write(domain::register_value_object const& dto) -> void {
  return singleton<detail::register_>()->write(dto.get_value(), dto.get_timestamp());
}

auto register_repository::reset() -> void {
  return singleton<detail::register_>()->reset();
}

auto register_repository::read() -> std::optional<domain::register_value_object> {
  std::optional<detail::register_::value> const value = singleton<detail::register_>()->read();
  if (value.has_value()) {
    return domain::register_value_object(
        domain::register_value(value->value), domain::register_timestamp(value->timestamp));
  }
  return std::nullopt;
}

}  // namespace dsac::infrastructure::inmemory