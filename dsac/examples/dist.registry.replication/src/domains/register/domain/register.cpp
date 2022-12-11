#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

namespace dsac::domain {

register_dto::register_dto(std::int32_t value, std::size_t timestamp)
  : value_(value)
  , timestamp_(timestamp) {
}

register_dto register_dto::hydrate(std::int32_t value, std::size_t timestamp) noexcept {
  return {value, timestamp};
}

auto register_dto::get_value() const noexcept -> std::int32_t {
  return value_;
}

auto register_dto::get_timestamp() const noexcept -> std::size_t {
  return timestamp_;
}

bool operator>(const register_dto& lhs, const register_dto& rhs) {
  return lhs.get_timestamp() > rhs.get_timestamp();
}

}  // namespace dsac::domain