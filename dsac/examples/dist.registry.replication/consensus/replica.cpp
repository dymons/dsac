#include <examples/dist.registry.replication/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <mutex>
#include <optional>

namespace {

const std::string kUnspecifiedValue = "__unspecified_value";

class register_ final {
public:
  struct value final {
    int         value;
    std::size_t timestamp;
  };

  [[nodiscard]] std::optional<value> get() {
    std::shared_lock guard(mutex_);
    return register_;
  }

  void set(int const value, std::size_t const& timestamp) {
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

  void reset() {
    std::unique_lock guard(mutex_);
    register_.reset();
  }

private:
  std::optional<value> register_;
  std::shared_mutex    mutex_;
};

}  // namespace

namespace dsac {

auto replica_set::execute(request request) -> expected<response, std::string> {
  if (!request.value.has_value() || !request.timestamp.has_value()) {
    return dsac::make_unexpected("Input data is incorrect for consensus algorithm");
  }

  singleton<register_>()->set(request.value.value(), request.timestamp.value());

  // We always confirm the client's record, even if we ignore it by timestamp.
  return response{};
}

auto replica_get::execute([[maybe_unused]] request request) -> expected<response, std::string> {
  std::optional<register_::value> v = singleton<register_>()->get();
  if (v.has_value()) {
    return response{.value = v->value, .timestamp = v->timestamp};
  }

  return dsac::make_unexpected(kUnspecifiedValue);
}

auto replica_reset::execute([[maybe_unused]] request request) -> expected<response, std::string> {
  singleton<register_>()->reset();
  return response{};
}

}  // namespace dsac
