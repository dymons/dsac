#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac::domain {

class replica_base {
public:
  replica_base()                                   = default;
  replica_base(const replica_base&)                = delete;
  replica_base(replica_base&&) noexcept            = delete;
  replica_base& operator=(const replica_base&)     = delete;
  replica_base& operator=(replica_base&&) noexcept = delete;
  virtual ~replica_base()                          = default;

  virtual auto async_write(domain::register_value_object const& object) -> future<void*> = 0;

  virtual auto async_read() -> future<domain::register_value_object> = 0;
};

using replica_ref = shared_ptr<replica_base>;

}  // namespace dsac::domain
