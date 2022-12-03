#include <examples/dist.registry.replication/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <chrono>
#include <map>
#include <mutex>
#include <optional>

namespace {

const std::string kUnspecifiedValue = "__unspecified_value";

class register_ final {
public:
  struct value final {
    std::string                                        value;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
  };

  [[nodiscard]] std::optional<value> get() {
    std::shared_lock guard(mutex_);
    return register_;
  }

  void set(std::string const& value, std::chrono::time_point<std::chrono::system_clock> const& timestamp) {
    std::unique_lock guard(mutex_);
    register_ = register_::value{
        .value     = value,
        .timestamp = timestamp,
    };
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
  return response{.value = request.value, .timestamp = request.timestamp};
}

auto replica_get::execute([[maybe_unused]] request request) -> expected<response, std::string> {
  std::optional<register_::value> v = singleton<register_>()->get();
  if (v.has_value()) {
    return response{.value = v->value, .timestamp = v->timestamp};
  }

  return dsac::make_unexpected(kUnspecifiedValue);
}

}  // namespace dsac
