#include <examples/dist.consensus.abd/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <map>
#include <mutex>
#include <optional>

namespace {

const std::string kUnexpectedKey    = "__unexpected_key";
const std::string kUnexpectedValue  = "__unexpected_value";
const std::string kUnspecifiedValue = "__unspecified_value";
const std::string kKey              = "key";
const std::string kValue            = "value";

class key_value_store final {
  std::map<std::string, std::string> storage_;
  std::shared_mutex                  mutex_;

public:
  [[nodiscard]] std::optional<std::string> read(std::string const& key) {
    std::shared_lock guard(mutex_);
    if (storage_.contains(key)) {
      return storage_[key];
    }
    return std::nullopt;
  }

  void write(std::string const& key, std::string const& value) {
    std::unique_lock guard(mutex_);
    storage_[key] = value;
  }
};

auto make_response(std::optional<std::string> const& value) -> dsac::json {
  return dsac::json{{"value", value.value_or(kUnspecifiedValue)}};
}

}  // namespace

namespace dsac {

auto replica_set::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  if (!request.contains(kKey) || !request[kKey].is_string()) {
    return dsac::make_unexpected(kUnexpectedKey);
  }
  if (!request.contains(kValue) || !request[kValue].is_string()) {
    return dsac::make_unexpected(kUnexpectedValue);
  }

  singleton<key_value_store>()->write(request[kKey].get<std::string>(), request[kValue].get<std::string>());

  return "";
}

auto replica_get::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  if (!request.contains(kKey) || !request[kKey].is_string()) {
    return dsac::make_unexpected(kUnexpectedKey);
  }
  return make_response(singleton<key_value_store>()->read(request[kKey].get<std::string>()));
}

}  // namespace dsac
