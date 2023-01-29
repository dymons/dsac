#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac::domain {

class replica_base {
public:
  // Constructors

  /*!
    \brief
        Default constructor.
  */
  replica_base() = default;

  /*!
    \brief
        Copy constructor.
  */
  replica_base(const replica_base&) = delete;

  /*!
    \brief
        Move constructor.
  */
  replica_base(replica_base&&) noexcept = delete;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  virtual ~replica_base() = default;

  // Assignment

  /*!
    \brief
        Copy conversion operator.
  */
  replica_base& operator=(const replica_base&) = delete;

  /*!
    \brief
        Move conversion operator.
  */
  replica_base& operator=(replica_base&&) noexcept = delete;

  // Observers

  /*!
    \brief
        Read current value of register from the replica
  */
  virtual auto async_read() -> future<domain::register_value_object> = 0;

  // Modifiers

  /*!
    \brief
        Write a new register value to the replica

    \param object
        A new register value
  */
  virtual auto async_write(domain::register_value_object const& object) -> future<void*> = 0;
};

using replica_ref = shared_ptr<replica_base>;

}  // namespace dsac::domain
