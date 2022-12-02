#include <examples/dist.consensus.abd/consensus/replica.hpp>

#include <dsac/pattern/singleton/singleton.hpp>

#include <atomic>

namespace {

class register_ final {
  std::atomic<std::int64_t> storage_;

public:
  [[nodiscard]] std::int64_t read() const noexcept {
    return storage_.load();
  }

  void write(std::int64_t value) noexcept {
    storage_.store(value);
  }
};

}  // namespace

namespace dsac {

auto replica_set::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return {};
}

auto replica_get::execute([[maybe_unused]] json request) -> expected<json, std::string> {
  return singleton<register_>()->read();
}

}  // namespace dsac
