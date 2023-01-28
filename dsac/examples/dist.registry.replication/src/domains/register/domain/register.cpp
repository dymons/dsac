#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

namespace dsac::domain {

auto register_value_object::get_value() const noexcept -> register_value {
  return value_;
}

auto register_value_object::get_timestamp() const noexcept -> register_timestamp {
  return timestamp_;
}

bool operator>(const register_value_object& lhs, const register_value_object& rhs) {
  return lhs.get_timestamp().get() > rhs.get_timestamp().get();
}

bool operator==(register_value_object const& lhs, register_value_object const& rhs) {
  return (lhs.get_timestamp().get() == rhs.get_timestamp().get()) && (lhs.get_value().get() == rhs.get_value().get());
}

}  // namespace dsac::domain