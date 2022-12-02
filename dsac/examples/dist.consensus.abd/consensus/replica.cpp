#include <examples/dist.consensus.abd/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <chrono>
#include <map>
#include <mutex>
#include <optional>

namespace {

const std::string kUnexpectedKey       = "__unexpected_key";
const std::string kUnexpectedValue     = "__unexpected_value";
const std::string kUnexpectedTimestamp = "__unexpected_timestamp";
const std::string kUnspecifiedValue    = "__unspecified_value";
const std::string kKey                 = "key";
const std::string kValue               = "value";
const std::string kTimestamp           = "timestamp";

class key_value_store final {
public:
  struct value final {
    std::string                                        value;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
  };

  [[nodiscard]] std::optional<value> get(std::string const& key) {
    std::shared_lock guard(mutex_);
    if (storage_.contains(key)) {
      return storage_[key];
    }
    return std::nullopt;
  }

  void set(std::string const& key, value&& value_) {
    std::unique_lock guard(mutex_);
    storage_[key] = std::move(value_);
  }

private:
  std::map<std::string, value> storage_;
  std::shared_mutex            mutex_;
};

auto make_response(std::optional<key_value_store::value> const& record) -> dsac::json {
  if (!record.has_value()) {
    return dsac::json{{kValue, kUnspecifiedValue}, {kTimestamp, 0UL}};
  }
  return dsac::json{{kValue, record->value}, {kTimestamp, record->timestamp.time_since_epoch().count()}};
}

}  // namespace

namespace dsac {

auto replica_set::execute(json request) -> expected<json, std::string> {
  if (!request.contains(kKey) || !request[kKey].is_string()) {
    return dsac::make_unexpected(kUnexpectedKey);
  }
  if (!request.contains(kValue) || !request[kValue].is_string()) {
    return dsac::make_unexpected(kUnexpectedValue);
  }
  if (!request.contains(kTimestamp) || !request[kTimestamp].is_number_unsigned()) {
    return dsac::make_unexpected(kUnexpectedTimestamp);
  }

  key_value_store::value value{
      .value     = request[kValue].get<std::string>(),
      .timestamp = std::chrono::time_point<std::chrono::steady_clock>{
          static_cast<const std::chrono::duration<long, std::ratio<1, 1000000000>>>(request[kTimestamp].get<int>())}};

  return (singleton<key_value_store>()->set(request[kKey].get<std::string>(), std::move(value)), "");
}

auto replica_get::execute(json request) -> expected<json, std::string> {
  if (!request.contains(kKey) || !request[kKey].is_string()) {
    return dsac::make_unexpected(kUnexpectedKey);
  }
  return make_response(singleton<key_value_store>()->get(request[kKey].get<std::string>()));
}

}  // namespace dsac
