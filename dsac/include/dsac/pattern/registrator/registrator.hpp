#pragma once

#include <dsac/pattern/singleton/singleton.hpp>

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <algorithm>

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
  std::unique_ptr<BaseComponent> construct(Args&&... args) const override {
    return std::make_unique<DerivedComponent>(std::forward<Args>(args)...);
  }
};

template <typename ComponentBase, typename... Args>
class factory_base {
protected:
  template <typename DerivedComponent>
  void register_component(const std::string& name) {
    std::unique_lock guard(mutex_);
    if (!constructors_.emplace(name, new factory_constructor<ComponentBase, DerivedComponent, Args...>).second) {
      throw factory_component_duplicate{name};
    }
  }

  std::set<std::string> get_keys() const {
    std::shared_lock guard(mutex_);

    std::set<std::string> keys;
    std::ranges::transform(constructors_, std::inserter(keys, keys.begin()), [](auto&& ctr) { return ctr.first; });

    return keys;
  }

  factory_constructor_base<ComponentBase, Args...>* get_constructor_unsafe(const std::string& key) const {
    std::shared_lock guard(mutex_);
    if (auto constructor = constructors_.find(key); constructor != constructors_.end()) {
      return constructor->second.get();
    }
    return nullptr;
  }

  bool contains(const std::string& key) const {
    std::shared_lock guard(mutex_);
    return constructors_.find(key) != constructors_.end();
  }

private:
  std::map<std::string, std::unique_ptr<factory_constructor_base<ComponentBase, Args...>>> constructors_;
  mutable std::shared_mutex                                                                mutex_;
};

template <typename ComponentBase, typename... Args>
class factory final : public factory_base<ComponentBase, Args...> {
  std::unique_ptr<ComponentBase> construct_impl(const std::string& name, Args&&... args) const {
    factory_constructor_base<ComponentBase, Args...>* constructor = this->get_constructor_unsafe(name);
    return constructor == nullptr ? nullptr : constructor->construct(std::forward<Args>(args)...);
  }

public:
  [[nodiscard]] static bool contains(const std::string& key) {
    return singleton<factory<ComponentBase, Args...>>()->factory_base<ComponentBase, Args...>::contains(key);
  }

  [[nodiscard]] static std::unique_ptr<ComponentBase> construct(const std::string& key, Args... args) {
    return singleton<factory<ComponentBase, Args...>>()->construct_impl(key, std::forward<Args>(args)...);
  }

  [[nodiscard]] static std::set<std::string> get_registered_keys() {
    return singleton<factory<ComponentBase, Args...>>()->get_keys();
  }

  template <typename DerivedComponent>
  class [[nodiscard]] registractor {
  public:
    explicit registractor(const std::string& key) {
      singleton<factory<ComponentBase, Args...>>()->template register_component<DerivedComponent>(key);
    }

    registractor()
      : registractor(DerivedComponent::get_type_name()) {
    }
  };
};

}  // namespace dsac