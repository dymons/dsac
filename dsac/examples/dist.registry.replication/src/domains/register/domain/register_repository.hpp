#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/memory/not_null.hpp>
#include <dsac/memory/shared_ptr.hpp>

#include <optional>

namespace dsac::domain {

class register_repository_base {
public:
  // Constructors

  /*!
    \brief
        Default constructor.
  */
  register_repository_base() = default;

  /*!
    \brief
        Copy constructor.
  */
  register_repository_base(const register_repository_base&) = delete;

  /*!
    \brief
        Move constructor.
  */
  register_repository_base(register_repository_base&&) noexcept = delete;

  // Destructor

  virtual ~register_repository_base() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  register_repository_base& operator=(const register_repository_base&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  register_repository_base& operator=(register_repository_base&&) noexcept = delete;

  // Observers

  /*!
    \brief
        Read current value of register from the repository
  */
  [[nodiscard]] virtual auto read() -> std::optional<register_value_object> = 0;

  // Modifiers

  /*!
    \brief
        Write a new register value to the repository

    \param object
        A new register value
  */
  virtual auto write(register_value_object const& object) -> void = 0;

  /*!
    \brief
        Reset the current state of the register. This method is used to reset the system to its original state
        and write integration tests.
  */
  virtual auto reset() -> void = 0;
};

using register_repository_ref = not_null<shared_ptr<register_repository_base>>;

}  // namespace dsac::domain
