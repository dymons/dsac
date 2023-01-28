#include <examples/dist.registry.replication/src/domains/register/infrastructure/inmemory/detail/register.hpp>

#include <mutex>
#include <shared_mutex>

namespace dsac::infrastructure::inmemory::detail {

class register_::pimpl {
  std::optional<value> register_;
  std::shared_mutex    mutex_;

public:
  auto write(std::int32_t value, std::size_t timestamp) -> void {
    std::unique_lock guard(mutex_);

    bool const is_register_setup = register_.has_value();
    bool const is_register_olden = is_register_setup && register_->timestamp < timestamp;
    if (!is_register_setup || is_register_olden) {
      register_ = register_::value{
          .value     = value,
          .timestamp = timestamp,
      };
    }
  }

  auto read() -> std::optional<value> {
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

auto register_::write(std::int32_t value, std::size_t timestamp) -> void {
  return pimpl_->write(value, timestamp);
}
auto register_::read() -> std::optional<value> {
  return pimpl_->read();
}
auto register_::reset() -> void {
  return pimpl_->reset();
}

}  // namespace dsac::infrastructure::inmemory::detail