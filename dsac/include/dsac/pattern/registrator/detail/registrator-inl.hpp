#ifndef PATTERN_REGISTRATOR_HPP
#error This file may only be included from registrator.hpp
#endif

#include <dsac/pattern/singleton/singleton.hpp>

#include <algorithm>
#include <mutex>

namespace dsac {

template <typename BaseComponent, typename DerivedComponent, typename... Args>
std::unique_ptr<BaseComponent> factory_constructor<BaseComponent, DerivedComponent, Args...>::construct(Args&&... args
) const {
  return std::make_unique<DerivedComponent>(std::forward<Args>(args)...);
}

template <typename BaseComponent, typename... Args>
template <typename DerivedComponent>
void factory_base<BaseComponent, Args...>::factory_register(const std::string& component_name) {
  std::unique_lock guard(mutex_);

  if (constructors_.contains(component_name)) [[unlikely]] {
    throw factory_component_duplicate{component_name};
  }

  constructors_.emplace(component_name, new factory_constructor<BaseComponent, DerivedComponent, Args...>{});
}

template <typename BaseComponent, typename... Args>
std::set<std::string> factory_base<BaseComponent, Args...>::get_factory_keys() const {
  std::shared_lock guard(mutex_);

  std::set<std::string> keys;
  std::ranges::transform(constructors_, std::inserter(keys, keys.begin()), [](auto&& ctr) { return ctr.first; });

  return keys;
}

template <typename BaseComponent, typename... Args>
factory_constructor_base<BaseComponent, Args...>* factory_base<BaseComponent, Args...>::get_factory_constructor_unsafe(
    const std::string& component_name
) const {
  std::shared_lock guard(mutex_);
  if (auto constructor = constructors_.find(component_name); constructor != constructors_.end()) {
    return constructor->second.get();
  }
  return nullptr;
}

template <typename BaseComponent, typename... Args>
bool factory_base<BaseComponent, Args...>::factory_contains(const std::string& component_name) const {
  std::shared_lock guard(mutex_);
  return constructors_.find(component_name) != constructors_.end();
}

template <typename BaseComponent, typename... Args>
std::unique_ptr<BaseComponent> factory<BaseComponent, Args...>::construct_impl(
    const std::string& component_name, Args&&... args
) const {
  factory_constructor_base<BaseComponent, Args...>* constructor = this->get_factory_constructor_unsafe(component_name);
  return constructor == nullptr ? nullptr : constructor->construct(std::forward<Args>(args)...);
}

template <typename BaseComponent, typename... Args>
bool factory<BaseComponent, Args...>::contains(const std::string& component_name) {
  return singleton<factory<BaseComponent, Args...>>()->factory_base<BaseComponent, Args...>::factory_contains(
      component_name
  );
}

template <typename BaseComponent, typename... Args>
std::unique_ptr<BaseComponent> factory<BaseComponent, Args...>::construct(
    const std::string& component_name, Args... args
) {
  return singleton<factory<BaseComponent, Args...>>()->construct_impl(component_name, std::forward<Args>(args)...);
}

template <typename BaseComponent, typename... Args>
std::set<std::string> factory<BaseComponent, Args...>::get_registered_keys() {
  return singleton<factory<BaseComponent, Args...>>()->get_factory_keys();
}

template <typename BaseComponent, typename... Args>
template <typename DerivedComponent>
factory<BaseComponent, Args...>::registractor<DerivedComponent>::registractor(const std::string& component_name) {
  singleton<factory<BaseComponent, Args...>>()->template factory_register<DerivedComponent>(component_name);
}

template <typename BaseComponent, typename... Args>
template <typename DerivedComponent>
factory<BaseComponent, Args...>::registractor<DerivedComponent>::registractor()
  : registractor(DerivedComponent::get_type_name()) {
}

}  // namespace dsac
