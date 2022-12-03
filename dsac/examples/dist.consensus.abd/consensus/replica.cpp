#include <examples/dist.consensus.abd/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <chrono>
#include <map>
#include <mutex>
#include <optional>

namespace {

const std::string kUnspecifiedValue = "__unspecified_value";

class key_value_store final {
public:
  struct value final {
    std::string                                        value;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
  };

  [[nodiscard]] std::optional<value> get(std::string const& key) {
    std::shared_lock guard(mutex_);
    if (storage_.contains(key)) {
      return storage_[key];
    }
    return std::nullopt;
  }

  void set(
      std::string const&                                        key,
      std::string const&                                        value,
      std::chrono::time_point<std::chrono::system_clock> const& timestamp) {
    std::unique_lock guard(mutex_);
    storage_[key] = key_value_store::value{
        .value     = value,
        .timestamp = timestamp,
    };
  }

private:
  std::map<std::string, value> storage_;
  std::shared_mutex            mutex_;
};

}  // namespace

namespace dsac {

auto replica_set::execute(request request) -> expected<response, std::string> {
  if (!request.key.has_value() || !request.value.has_value() || !request.timestamp.has_value()) {
    return dsac::make_unexpected("Input data is incorrect for consensus algorithm");
  }

  singleton<key_value_store>()->set(request.key.value(), request.value.value(), request.timestamp.value());
  return response{.value = request.value, .timestamp = request.timestamp};
}

auto replica_get::execute(request request) -> expected<response, std::string> {
  if (!request.key.has_value()) {
    return dsac::make_unexpected("Input data is incorrect for consensus algorithm");
  }

  std::optional<key_value_store::value> v = singleton<key_value_store>()->get(request.key.value());
  if (v.has_value()) {
    return response{.value = v->value, .timestamp = v->timestamp};
  }

  return dsac::make_unexpected(kUnspecifiedValue);
}

}  // namespace dsac
