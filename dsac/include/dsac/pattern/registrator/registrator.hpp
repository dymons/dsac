#pragma once

#include <dsac/pattern/singleton/singleton.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
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
  explicit factory_component_duplicate(const std::string& component);
};

template <typename BaseComponent, typename... Args>
class factory_constructor_base {
public:
  virtual std::unique_ptr<BaseComponent> construct(Args&&... args) const = 0;

  factory_constructor_base()                                               = default;
  factory_constructor_base(const factory_constructor_base&)                = default;
  factory_constructor_base(factory_constructor_base&&) noexcept            = default;
  factory_constructor_base& operator=(const factory_constructor_base&)     = default;
  factory_constructor_base& operator=(factory_constructor_base&&) noexcept = default;
  virtual ~factory_constructor_base()                                      = default;
};

template <typename BaseComponent, typename DerivedComponent, typename... Args>
class factory_constructor final : public factory_constructor_base<BaseComponent, Args...> {
  std::unique_ptr<BaseComponent> construct(Args&&... args) const override;
};

template <typename ComponentBase, typename... Args>
class factory_base {
protected:
  template <typename DerivedComponent>
  auto component_register(const std::string& name) -> void;

  auto component_constructor(const std::string& key) const -> factory_constructor_base<ComponentBase, Args...>*;

  auto component_keys() const -> std::set<std::string>;

  auto component_contains(const std::string& key) const -> bool;

private:
  std::map<std::string, std::unique_ptr<factory_constructor_base<ComponentBase, Args...>>> constructors_;
  mutable std::shared_mutex                                                                mutex_;
};

template <typename ComponentBase, typename... Args>
class factory final : public factory_base<ComponentBase, Args...> {
  std::unique_ptr<ComponentBase> construct_impl(const std::string& name, Args&&... args) const;

public:
  template <typename DerivedComponent>
  class [[nodiscard]] registractor;

  [[nodiscard]] static auto contains(const std::string& key) -> bool;

  [[nodiscard]] static auto construct(const std::string& key, Args... args) -> std::unique_ptr<ComponentBase>;

  [[nodiscard]] static auto get_registered_keys() -> std::set<std::string>;
};

template <typename ComponentBase, typename... Args>
template <typename DerivedComponent>
class [[nodiscard]] factory<ComponentBase, Args...>::registractor {
public:
  explicit registractor(const std::string& key);

  registractor();
};

}  // namespace dsac

#define PATTERN_REGISTRATOR_HPP
#include <dsac/pattern/registrator/detail/registrator-inl.hpp>
#undef PATTERN_REGISTRATOR_HPP
