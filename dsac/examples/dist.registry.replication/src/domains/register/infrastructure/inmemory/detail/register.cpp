#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/detail/register.hpp>

#include <mutex>
#include <shared_mutex>

namespace dsac::infrastructure::inmemory::detail {

class register_::pimpl {
  std::optional<domain::register_value_object> register_;
  std::shared_mutex                            mutex_;

public:
  auto write(domain::register_value_object new_register) -> void {
    std::unique_lock guard(mutex_);

    bool const is_register_setup = register_.has_value();
    bool const is_register_olden =
        is_register_setup && register_->get_timestamp().get() < new_register.get_timestamp().get();
    if (!is_register_setup || is_register_olden) {
      register_ = new_register;
    }
  }

  auto read() -> std::optional<domain::register_value_object> {
    std::shared_lock guard(mutex_);
    return register_;
  }

  auto reset() -> void {
    std::unique_lock guard(mutex_);
    register_.reset();
  }
};

register_::register_()
  : pimpl_(make_shared<register_::pimpl>()) {
}

auto register_::write(domain::register_value_object _register) -> void {
  return pimpl_->write(_register);
}
auto register_::read() -> std::optional<domain::register_value_object> {
  return pimpl_->read();
}
auto register_::reset() -> void {
  return pimpl_->reset();
}

}  // namespace dsac::infrastructure::inmemory::detail