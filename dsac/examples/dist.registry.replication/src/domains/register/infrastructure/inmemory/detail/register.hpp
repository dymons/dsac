#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/memory/shared_ptr.hpp>

#include <cstdint>
#include <optional>

namespace dsac::infrastructure::inmemory::detail {

class register_ final {
public:
  register_();

  auto write(domain::register_value_object _register) -> void;
  auto read() -> std::optional<domain::register_value_object>;
  auto reset() -> void;

private:
  class pimpl;
  shared_ptr<pimpl> pimpl_;
};

}  // namespace dsac::infrastructure::inmemory::detail