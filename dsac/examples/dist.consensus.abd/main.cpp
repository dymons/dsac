#include <dsac/functional/expected.hpp>
#include <dsac/pattern/registrator/registrator.hpp>

#include <nlohmann/json.hpp>

namespace dsac {

using nlohmann::json;

}  // namespace dsac

namespace {

class abd {
public:
  using factory = dsac::factory<abd>;

  abd()                          = default;
  abd(const abd&)                = default;
  abd(abd&&) noexcept            = default;
  abd& operator=(const abd&)     = default;
  abd& operator=(abd&&) noexcept = default;
  virtual ~abd()                 = default;

  virtual auto handle(dsac::json request) -> dsac::expected<dsac::json, std::string> = 0;
};

class coordinator_set final : public abd {
  static const inline factory::registractor<coordinator_set> kRegistractor;

  auto handle([[maybe_unused]] dsac::json request) -> dsac::expected<dsac::json, std::string> override {
    return {};
  }

public:
  static std::string get_type_name() {
    return "coordinator/set";
  }
};

class coordinator_get final : public abd {
  static const inline factory::registractor<coordinator_get> kRegistractor;

  auto handle([[maybe_unused]] dsac::json request) -> dsac::expected<dsac::json, std::string> override {
    return {};
  }

public:
  static std::string get_type_name() {
    return "coordinator/get";
  }
};

class replica_set final : public abd {
  static const inline factory::registractor<replica_set> kRegistractor;

  auto handle([[maybe_unused]] dsac::json request) -> dsac::expected<dsac::json, std::string> override {
    return {};
  }

public:
  static std::string get_type_name() {
    return "replica/set";
  }
};

class replica_get final : public abd {
  static const inline factory::registractor<replica_get> kRegistractor;

  auto handle([[maybe_unused]] dsac::json request) -> dsac::expected<dsac::json, std::string> override {
    return {};
  }

public:
  static std::string get_type_name() {
    return "replica/get";
  }
};

}  // namespace

int main() {
}
