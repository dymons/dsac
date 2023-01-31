#pragma once

#include <map>
#include <memory>
#include <set>
#include <shared_mutex>
#include <string>

namespace dsac {

class factory_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class factory_component_duplicate final : public factory_exception {
public:
  explicit factory_component_duplicate(const std::string& component_name);
};

template <typename BaseComponent, typename... Args>
class factory_constructor_base {
public:
  // Constructors

  /*!
    \brief
        Constructor.
  */
  factory_constructor_base() = default;

  /*!
    \brief
        Copy constructor.
  */
  factory_constructor_base(const factory_constructor_base&) = default;

  /*!
    \brief
        Move constructor.
  */
  factory_constructor_base(factory_constructor_base&&) noexcept = default;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  virtual ~factory_constructor_base() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  factory_constructor_base& operator=(const factory_constructor_base&) = default;

  /*!
    \brief
        Move conversion constructor.
  */
  factory_constructor_base& operator=(factory_constructor_base&&) noexcept = default;

  // Modifiers

  /*!
    \brief
        Creating a custom component in a factory

    \param args
        Arguments for creating a component

    \ingroup
        DsacPattern
  */
  virtual auto construct(Args&&... args) const -> std::unique_ptr<BaseComponent> = 0;
};

template <typename BaseComponent, typename DerivedComponent, typename... Args>
class factory_constructor final : public factory_constructor_base<BaseComponent, Args...> {
  auto construct(Args&&... args) const -> std::unique_ptr<BaseComponent> final;
};

template <typename BaseComponent, typename... Args>
class factory_base {
protected:
  // Observers

  /*!
    \brief
        Get a factory to create a component

    \param component_name
        Name of the user component

    \ingroup
        DsacPattern
  */
  // clang-format off
  auto get_factory_constructor_unsafe(const std::string& component_name) const -> factory_constructor_base<BaseComponent, Args...>*;
  // clang-format on

  /*!
    \brief
        Get all the names of user components that have been added

    \ingroup
        DsacPattern
  */
  auto get_factory_keys() const -> std::set<std::string>;

  /*!
    \brief
        Check that the user component is contained in the component factory

    \ingroup
        DsacPattern
  */
  auto factory_contains(const std::string& component_name) const -> bool;

  // Modifiers

  /*!
    \brief
        Register a new user component in the factory

    \param component_name
        Name of the user component

    \throw dsac::factory_component_duplicate
        Attempt to create a duplicate user component in the factory

    \ingroup
        DsacPattern
  */
  template <typename DerivedComponent>
  auto factory_register(const std::string& component_name) -> void;

private:
  /// A list of factories for creating custom components
  std::map<std::string, std::unique_ptr<factory_constructor_base<BaseComponent, Args...>>> constructors_;

  /// Does work with a list of factories thread-safe
  mutable std::shared_mutex mutex_;
};

template <typename BaseComponent, typename... Args>
class factory final : public factory_base<BaseComponent, Args...> {
  std::unique_ptr<BaseComponent> construct_impl(const std::string& component_name, Args&&... args) const;

public:
  template <typename DerivedComponent>
  class [[nodiscard]] registractor;

  // Observers

  /*!
    \brief
        Check that the user component is contained in the component factory

    \param component_name
        Name of the user component

    \ingroup
        DsacPattern
  */
  [[nodiscard]] static auto contains(const std::string& component_name) -> bool;

  /*!
    \brief
        Get all the names of user components that have been added

    \ingroup
        DsacPattern
  */
  [[nodiscard]] static auto get_registered_keys() -> std::set<std::string>;

  // Modifiers

  /*!
    \brief
        Creating a user component using a factory

    \param component_name
        Name of the user component

    \param args
        Arguments for component construction

    \ingroup
        DsacPattern
  */
  // clang-format off
  [[nodiscard]] static auto construct(const std::string& component_name, Args... args) -> std::unique_ptr<BaseComponent>;
  // clang-format on
};

template <typename BaseComponent, typename... Args>
template <typename DerivedComponent>
class [[nodiscard]] factory<BaseComponent, Args...>::registractor {
public:
  // Constructors

  /*!
    \brief
        Default constructor, registers a user component in the factory by DerivedComponent::get_type_name
  */
  registractor();

  /*!
    \brief
        User constructor, registers a user component in the factory by component_name

    \param component_name
        Name of the user component
  */
  explicit registractor(const std::string& component_name);
};

}  // namespace dsac

#define PATTERN_REGISTRATOR_HPP
#include <dsac/pattern/registrator/detail/registrator-inl.hpp>
#undef PATTERN_REGISTRATOR_HPP
