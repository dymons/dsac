#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

namespace dsac::domain {

auto register_value_object::get_value() const noexcept -> std::int32_t {
  return value_.get();
}

auto register_value_object::get_timestamp() const noexcept -> std::size_t {
  return timestamp_.get();
}

bool operator>(const register_value_object& lhs, const register_value_object& rhs) {
  return lhs.get_timestamp() > rhs.get_timestamp();
}

bool operator==(register_value_object const& lhs, register_value_object const& rhs) {
  return (lhs.get_timestamp() == rhs.get_timestamp()) && (lhs.get_value() == rhs.get_value());
}

}  // namespace dsac::domain