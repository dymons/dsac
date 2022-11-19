#ifndef PATTERN_REGISTRATOR_HPP
#error This file may only be included from registrator.hpp
#endif

namespace dsac {

template <typename BaseComponent, typename DerivedComponent, typename... Args>
std::unique_ptr<BaseComponent> factory_constructor<BaseComponent, DerivedComponent, Args...>::construct(
    Args&&... args) const {
  return std::make_unique<DerivedComponent>(std::forward<Args>(args)...);
}

template <typename ComponentBase, typename... Args>
template <typename DerivedComponent>
void factory_base<ComponentBase, Args...>::component_register(const std::string& name) {
  std::unique_lock guard(mutex_);
  if (!constructors_.emplace(name, new factory_constructor<ComponentBase, DerivedComponent, Args...>).second) {
    throw factory_component_duplicate{name};
  }
}

template <typename ComponentBase, typename... Args>
std::set<std::string> factory_base<ComponentBase, Args...>::component_keys() const {
  std::shared_lock guard(mutex_);

  std::set<std::string> keys;
  std::ranges::transform(constructors_, std::inserter(keys, keys.begin()), [](auto&& ctr) { return ctr.first; });

  return keys;
}

template <typename ComponentBase, typename... Args>
factory_constructor_base<ComponentBase, Args...>* factory_base<ComponentBase, Args...>::component_constructor(
    const std::string& key) const {
  std::shared_lock guard(mutex_);
  if (auto constructor = constructors_.find(key); constructor != constructors_.end()) {
    return constructor->second.get();
  }
  return nullptr;
}

template <typename ComponentBase, typename... Args>
bool factory_base<ComponentBase, Args...>::component_contains(const std::string& key) const {
  std::shared_lock guard(mutex_);
  return constructors_.find(key) != constructors_.end();
}

template <typename ComponentBase, typename... Args>
std::unique_ptr<ComponentBase> factory<ComponentBase, Args...>::construct_impl(
    const std::string& name, Args&&... args) const {
  factory_constructor_base<ComponentBase, Args...>* constructor = this->component_constructor(name);
  return constructor == nullptr ? nullptr : constructor->construct(std::forward<Args>(args)...);
}

template <typename ComponentBase, typename... Args>
bool factory<ComponentBase, Args...>::contains(const std::string& key) {
  return singleton<factory<ComponentBase, Args...>>()->factory_base<ComponentBase, Args...>::component_contains(key);
}

template <typename ComponentBase, typename... Args>
std::unique_ptr<ComponentBase> factory<ComponentBase, Args...>::construct(const std::string& key, Args... args) {
  return singleton<factory<ComponentBase, Args...>>()->construct_impl(key, std::forward<Args>(args)...);
}

template <typename ComponentBase, typename... Args>
std::set<std::string> factory<ComponentBase, Args...>::get_registered_keys() {
  return singleton<factory<ComponentBase, Args...>>()->component_keys();
}

template <typename ComponentBase, typename... Args>
template <typename DerivedComponent>
factory<ComponentBase, Args...>::registractor<DerivedComponent>::registractor(const std::string& key) {
  singleton<factory<ComponentBase, Args...>>()->template component_register<DerivedComponent>(key);
}

template <typename ComponentBase, typename... Args>
template <typename DerivedComponent>
factory<ComponentBase, Args...>::registractor<DerivedComponent>::registractor()
  : registractor(DerivedComponent::get_type_name()) {
}

}  // namespace dsac
